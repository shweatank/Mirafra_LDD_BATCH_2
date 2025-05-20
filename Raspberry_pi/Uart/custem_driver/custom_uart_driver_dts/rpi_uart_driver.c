#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/delay.h>

#define DRIVER_NAME "rpi_custom_uart"
#define DEVICE_NAME "rpiuart"
#define UART_BASE 0xFE201000 // PL011 UART base for Raspberry Pi 4
#define UART_SIZE 0x1000     // Memory region size

/* PL011 UART Registers */
#define UART_DR   0x00 // Data Register
#define UART_FR   0x18 // Flag Register
#define UART_IBRD 0x24 // Integer Baud Rate Divisor
#define UART_FBRD 0x28 // Fractional Baud Rate Divisor
#define UART_LCRH 0x2C // Line Control Register
#define UART_CR   0x30 // Control Register
#define UART_IMSC 0x38 // Interrupt Mask Set/Clear Register
#define UART_ICR  0x44 // Interrupt Clear Register

/* Flag Register Bits */
#define FR_TXFF (1 << 5) // Transmit FIFO Full
#define FR_RXFE (1 << 4) // Receive FIFO Empty

static dev_t dev_number;
static struct cdev cdev;
static struct class *class;
static struct device *device;
static void __iomem *uart_base;

static int uart_open(struct inode *inode, struct file *file) {
    return 0;
}

static int uart_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t uart_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char *kbuf;
    int i = 0;

    kbuf = kmalloc(count, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    while (i < count) {
        if (!(ioread32(uart_base + UART_FR) & FR_RXFE)) {
            kbuf[i++] = ioread32(uart_base + UART_DR) & 0xFF;
        } else {
            break; // No data available
        }
    }

    if (i > 0) {
        if (copy_to_user(buf, kbuf, i)) {
            kfree(kbuf);
            return -EFAULT;
        }
    }

    kfree(kbuf);
    return i;
}

static ssize_t uart_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    char *kbuf;
    int i;

    kbuf = kmalloc(count, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    if (copy_from_user(kbuf, buf, count)) {
        kfree(kbuf);
        return -EFAULT;
    }

    for (i = 0; i < count; i++) {
        while (ioread32(uart_base + UART_FR) & FR_TXFF) {
            udelay(1); // Wait for TX FIFO
        }
        iowrite32(kbuf[i], uart_base + UART_DR);
    }

    kfree(kbuf);
    return count;
}

static const struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .open = uart_open,
    .release = uart_release,
    .read = uart_read,
    .write = uart_write,
};

static void uart_init(void) {
    // Disable UART
    iowrite32(0, uart_base + UART_CR);

    // Set baud rate (9600 baud, 48MHz clock)
    iowrite32(31, uart_base + UART_IBRD); // 48MHz / (16 * 9600) ≈ 31.25
    iowrite32(16, uart_base + UART_FBRD); // Fractional: 0.25 * 64 ≈ 16

    // Set line control: 8 bits, no parity, 1 stop bit, FIFO enable
    iowrite32((1 << 4) | (1 << 5) | (1 << 6), uart_base + UART_LCRH);

    // Enable UART, TX, RX
    iowrite32((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    // Clear interrupts
    iowrite32(0x7FF, uart_base + UART_ICR);
}

static int uart_probe(struct platform_device *pdev) {
    int ret;

    // Map UART registers
    uart_base = devm_ioremap(&pdev->dev, UART_BASE, UART_SIZE);
    if (!uart_base) {
        dev_err(&pdev->dev, "Failed to map UART registers\n");
        return -ENOMEM;
    }

    // Initialize UART hardware
    uart_init();

    // Register character device
    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        dev_err(&pdev->dev, "Failed to allocate device number\n");
        return ret;
    }

    cdev_init(&cdev, &uart_fops);
    cdev.owner = THIS_MODULE;
    ret = cdev_add(&cdev, dev_number, 1);
    if (ret < 0) {
        dev_err(&pdev->dev, "Failed to add cdev\n");
        goto err_free_chrdev;
    }

    class = class_create(DEVICE_NAME);
    if (IS_ERR(class)) {
        dev_err(&pdev->dev, "Failed to create class\n");
        ret = PTR_ERR(class);
        goto err_del_cdev;
    }

    device = device_create(class, NULL, dev_number, NULL, DEVICE_NAME);
    if (IS_ERR(device)) {
        dev_err(&pdev->dev, "Failed to create device\n");
        ret = PTR_ERR(device);
        goto err_destroy_class;
    }

    dev_info(&pdev->dev, "Custom UART driver loaded\n");
    return 0;

err_destroy_class:
    class_destroy(class);
err_del_cdev:
    cdev_del(&cdev);
err_free_chrdev:
    unregister_chrdev_region(dev_number, 1);
    return ret;
}

static int uart_remove(struct platform_device *pdev) {
    device_destroy(class, dev_number);
    class_destroy(class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev_number, 1);
    dev_info(&pdev->dev, "Custom UART driver unloaded\n");
    return 0;
}

static const struct of_device_id uart_of_match[] = {
    { .compatible = "rpi,custom-uart" },
    { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, uart_of_match);

static struct platform_driver uart_driver = {
    .probe = uart_probe,
    .remove = uart_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = uart_of_match,
    },
};

module_platform_driver(uart_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xAI");
MODULE_DESCRIPTION("Custom UART Driver for Raspberry Pi");