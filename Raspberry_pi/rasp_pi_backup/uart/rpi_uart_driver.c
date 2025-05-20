#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>

/* PL011 UART Registers (BCM2711 for Raspberry Pi 4) */
#define UART_BASE 0xFE201000 /* Change to 0x3F201000 for Pi 3 */
#define UART_DR   0x00  /* Data Register */
#define UART_FR   0x18  /* Flag Register */
#define UART_IBRD 0x24  /* Integer Baud Rate Divisor */
#define UART_FBRD 0x28  /* Fractional Baud Rate Divisor */
#define UART_LCRH 0x2C  /* Line Control Register */
#define UART_CR   0x30  /* Control Register */
#define UART_IMSC 0x38  /* Interrupt Mask Set/Clear Register */
#define UART_ICR  0x44  /* Interrupt Clear Register */

/* Flag Register Bits */
#define UART_FR_TXFF (1 << 5) /* Transmit FIFO Full */
#define UART_FR_RXFE (1 << 4) /* Receive FIFO Empty */
#define UART_FR_BUSY (1 << 3) /* UART Busy */

/* Line Control Register Bits */
#define UART_LCRH_WLEN_8BIT (0x3 << 5) /* 8-bit word length */
#define UART_LCRH_FEN       (1 << 4)   /* Enable FIFOs */

/* Control Register Bits */
#define UART_CR_UARTEN (1 << 0)  /* UART Enable */
#define UART_CR_TXE    (1 << 8)  /* Transmit Enable */
#define UART_CR_RXE    (1 << 9)  /* Receive Enable */

/* Device parameters */
#define DRIVER_NAME "rpi_uart"
#define DEVICE_NAME "rpi_uart"
#define BUFFER_SIZE 1024

/* Module structure */
struct rpi_uart_dev {
    struct cdev cdev;
    void __iomem *regs; /* Mapped UART registers */
    dev_t dev_no;
    struct class *class;
    struct device *device;
};

static struct rpi_uart_dev *uart_dev;

/* UART register access macros */
#define uart_readl(offset)      readl(uart_dev->regs + offset)
#define uart_writel(val, offset) writel(val, uart_dev->regs + offset)

/* Initialize UART hardware */
static void rpi_uart_hw_init(void)
{
    /* Disable UART */
    uart_writel(0, UART_CR);

    /* Clear pending interrupts */
    uart_writel(0x7FF, UART_ICR);

    /* Set baud rate: 115200 @ 48MHz clock
     * Baud Divisor = 48MHz / (16 * 115200) = 26.0417
     * Integer part = 26, Fractional part = 0.0417 * 64 = 2.67 ~ 3
     */
    uart_writel(26, UART_IBRD);
    uart_writel(3, UART_FBRD);

    /* Set 8-bit data, no parity, 1 stop bit, enable FIFO */
    uart_writel(UART_LCRH_WLEN_8BIT | UART_LCRH_FEN, UART_LCRH);

    /* Enable UART, TX, and RX */
    uart_writel(UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE, UART_CR);

    /* Disable interrupts */
    uart_writel(0, UART_IMSC);
}

/* File operations */
static int rpi_uart_open(struct inode *inode, struct file *filp)
{
    filp->private_data = uart_dev;
    pr_info("rpi_uart: Device opened\n");
    return 0;
}

static int rpi_uart_release(struct inode *inode, struct file *filp)
{
    pr_info("rpi_uart: Device closed\n");
    return 0;
}

static ssize_t rpi_uart_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    size_t i = 0;
    char tmp;

    pr_info("rpi_uart: Attempting to read %zu bytes\n", count);
    while (i < count) {
        if (uart_readl(UART_FR) & UART_FR_RXFE) {
            pr_info("rpi_uart: RX FIFO empty\n");
            if (i == 0) /* Non-blocking read */
                break;
            else
                return i;
        }
        tmp = (char)(uart_readl(UART_DR) & 0xFF);
        pr_info("rpi_uart: Read byte 0x%02x\n", tmp);
        if (copy_to_user(buf + i, &tmp, 1)) {
            pr_err("rpi_uart: copy_to_user failed\n");
            return -EFAULT;
        }
        i++;
    }
    pr_info("rpi_uart: Read %zu bytes\n", i);
    return i;
}

static ssize_t rpi_uart_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    size_t i = 0;
    char tmp;

    pr_info("rpi_uart: Attempting to write %zu bytes\n", count);
    while (i < count) {
        if (uart_readl(UART_FR) & UART_FR_TXFF) {
            pr_info("rpi_uart: TX FIFO full\n");
            if (i == 0) /* Non-blocking write */
                break;
            else
                return i;
        }
        if (copy_from_user(&tmp, buf + i, 1)) {
            pr_err("rpi_uart: copy_from_user failed\n");
            return -EFAULT;
        }
        pr_info("rpi_uart: Writing byte 0x%02x\n", tmp);
        uart_writel(tmp, UART_DR);
        /* Wait for transmission to complete */
        while (uart_readl(UART_FR) & UART_FR_BUSY)
            cpu_relax();
        i++;
    }
    pr_info("rpi_uart: Wrote %zu bytes\n", i);
    return i;
}

static struct file_operations rpi_uart_fops = {
    .owner = THIS_MODULE,
    .open = rpi_uart_open,
    .release = rpi_uart_release,
    .read = rpi_uart_read,
    .write = rpi_uart_write,
};

/* Platform driver probe */
static int rpi_uart_probe(struct platform_device *pdev)
{
    int ret;

    /* Allocate device structure */
    uart_dev = devm_kzalloc(&pdev->dev, sizeof(struct rpi_uart_dev), GFP_KERNEL);
    if (!uart_dev) {
        dev_err(&pdev->dev, "Failed to allocate device structure\n");
        return -ENOMEM;
    }

    /* Map UART registers */
    uart_dev->regs = devm_ioremap(&pdev->dev, UART_BASE, 0x100);
    if (!uart_dev->regs) {
        dev_err(&pdev->dev, "Failed to map UART registers\n");
        return -ENOMEM;
    }

    /* Allocate device number */
    ret = alloc_chrdev_region(&uart_dev->dev_no, 0, 1, DEVICE_NAME);
    if (ret) {
        dev_err(&pdev->dev, "Failed to allocate device number\n");
        return ret;
    }

    /* Initialize cdev */
    cdev_init(&uart_dev->cdev, &rpi_uart_fops);
    uart_dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&uart_dev->cdev, uart_dev->dev_no, 1);
    if (ret) {
        dev_err(&pdev->dev, "Failed to add cdev\n");
        unregister_chrdev_region(uart_dev->dev_no, 1);
        return ret;
    }

    /* Create device class */
    uart_dev->class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(uart_dev->class)) {
        dev_err(&pdev->dev, "Failed to create class\n");
        cdev_del(&uart_dev->cdev);
        unregister_chrdev_region(uart_dev->dev_no, 1);
        return PTR_ERR(uart_dev->class);
    }

    /* Create device node */
    uart_dev->device = device_create(uart_dev->class, NULL, uart_dev->dev_no, NULL, DEVICE_NAME);
    if (IS_ERR(uart_dev->device)) {
        dev_err(&pdev->dev, "Failed to create device\n");
        class_destroy(uart_dev->class);
        cdev_del(&uart_dev->cdev);
        unregister_chrdev_region(uart_dev->dev_no, 1);
        return PTR_ERR(uart_dev->device);
    }

    /* Initialize UART hardware */
    rpi_uart_hw_init();

    dev_info(&pdev->dev, "RPi UART driver loaded\n");
    return 0;
}

/* Platform driver remove */
static int rpi_uart_remove(struct platform_device *pdev)
{
    device_destroy(uart_dev->class, uart_dev->dev_no);
    class_destroy(uart_dev->class);
    cdev_del(&uart_dev->cdev);
    unregister_chrdev_region(uart_dev->dev_no, 1);
    dev_info(&pdev->dev, "RPi UART driver unloaded\n");
    return 0;
}

/* Device tree matching */
static const struct of_device_id rpi_uart_of_match[] = {
    { .compatible = "rpi,uart-driver" },
    { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, rpi_uart_of_match);

/* Platform driver structure */
static struct platform_driver rpi_uart_driver = {
    .probe = rpi_uart_probe,
    .remove = rpi_uart_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = rpi_uart_of_match,
    },
};

/* Module init and exit */
static int __init rpi_uart_module_init(void)
{
    return platform_driver_register(&rpi_uart_driver);
}

static void __exit rpi_uart_module_exit(void)
{
    platform_driver_unregister(&rpi_uart_driver);
}

module_init(rpi_uart_module_init);
module_exit(rpi_uart_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grok");
MODULE_DESCRIPTION("Custom UART driver for Raspberry Pi");
