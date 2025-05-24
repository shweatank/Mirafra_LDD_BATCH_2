#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define DEVICE_NAME "bcm_uart"
#define UART0_BASE_PHYS 0xFE201000  // PL011 UART0 base address on BCM2711
#define UART_REG_SIZE  0x1000

// UART Registers (offsets from base)
#define UART_DR     0x00  // Data Register
#define UART_FR     0x18  // Flag Register
#define UART_FR_TXFF 0x20 // Transmit FIFO Full

static void __iomem *uart_base;
static dev_t dev_num;
static struct cdev uart_cdev;

#define UART_DR        0x00
#define UART_FR        0x18
#define UART_FR_RXFE   0x10  // Receive FIFO Empty
#define UART_FR_TXFF   0x20  // Transmit FIFO Full

static int uart_open(struct inode *inode, struct file *file)
{
    pr_info("BCM UART: device opened\n");
    return 0;
}

static int uart_release(struct inode *inode, struct file *file)
{
    pr_info("BCM UART: device closed\n");
    return 0;
}

static ssize_t uart_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    char kbuf[256];
    size_t i;

    if (len > sizeof(kbuf))
        len = sizeof(kbuf);

    for (i = 0; i < len; i++) {
        // Wait for data to be available
        while (readl(uart_base + UART_FR) & UART_FR_RXFE)
            cpu_relax();

        // Read from UART
        kbuf[i] = readl(uart_base + UART_DR) & 0xFF;
    }

    // Copy to user
    if (copy_to_user(buf, kbuf, len))
        return -EFAULT;

    return len;
}

static ssize_t uart_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    char kbuf[256];
    size_t i;

    if (len > sizeof(kbuf) - 1)
        return -EINVAL;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    for (i = 0; i < len; i++) {
        // Wait until UART is not full
        while (readl(uart_base + UART_FR) & UART_FR_TXFF)
            cpu_relax();

        writel(kbuf[i], uart_base + UART_DR);
    }
 
    pr_info("writting into uart\n");
    return len; 
}

static const struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .open = uart_open,
    .release = uart_release,
    .write = uart_write,
    .read  =  uart_read,
};

static int __init uart_init(void)
{
    int ret;

    pr_info("BCM UART: Initializing driver\n");

    // Map UART base address
    uart_base = ioremap(UART0_BASE_PHYS, UART_REG_SIZE);
    if (!uart_base) {
        pr_err("BCM UART: Failed to map UART registers\n");
        return -ENOMEM;
    }

    // Allocate device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret) {
        iounmap(uart_base);
        return ret;
    }

    cdev_init(&uart_cdev, &uart_fops);
    uart_cdev.owner = THIS_MODULE;

    ret = cdev_add(&uart_cdev, dev_num, 1);
    if (ret) {
        unregister_chrdev_region(dev_num, 1);
        iounmap(uart_base);
        return ret;
    }

    pr_info("BCM UART: Driver loaded (major=%d)\n", MAJOR(dev_num));
    return 0;
}

static void __exit uart_exit(void)
{
    cdev_del(&uart_cdev);
    unregister_chrdev_region(dev_num, 1);
    iounmap(uart_base);
    pr_info("BCM UART: Driver unloaded\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("team3");
MODULE_DESCRIPTION("Basic UART driver for BCM2711 (Raspberry Pi 4)");

