#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define UART_BASE 0xFE201000
#define UART_DR   0x00
#define UART_FR   0x18
#define UART_IBRD 0x24
#define UART_FBRD 0x28
#define UART_LCRH 0x2C
#define UART_CR   0x30
#define UART_IMSC 0x38
#define UART_ICR  0x44

#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)
#define UART_FR_BUSY (1 << 3)

#define UART_LCRH_WLEN_8BIT (0x3 << 5)
#define UART_LCRH_FEN       (1 << 4)

#define UART_CR_UARTEN (1 << 0)
#define UART_CR_TXE    (1 << 8)
#define UART_CR_RXE    (1 << 9)

#define DRIVER_NAME "rpi_uart"
#define DEVICE_NAME "rpi_uart"
#define BUFFER_SIZE 1024

//#define LED_GPIO 18
#define LED_GPIO 530 //physical pin 12 i.8 gpio is 18

struct rpi_uart_dev {
    struct cdev cdev;
    void __iomem *regs;
    dev_t dev_no;
    struct class *class;
    struct device *device;
};

static struct rpi_uart_dev *uart_dev;

#define uart_readl(offset)      readl(uart_dev->regs + offset)
#define uart_writel(val, offset) writel(val, uart_dev->regs + offset)

static void rpi_uart_hw_init(void)
{
    uart_writel(0, UART_CR);
    uart_writel(0x7FF, UART_ICR);
    uart_writel(26, UART_IBRD);
    uart_writel(3, UART_FBRD);
    uart_writel(UART_LCRH_WLEN_8BIT | UART_LCRH_FEN, UART_LCRH);
    uart_writel(UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE, UART_CR);
    uart_writel(0, UART_IMSC);
}

static int rpi_uart_open(struct inode *inode, struct file *filp)
{
    filp->private_data = uart_dev;
    return 0;
}

static int rpi_uart_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static void blink_led(void)
{
    gpio_set_value(LED_GPIO, 1);
    mdelay(50);
    gpio_set_value(LED_GPIO, 0);
}

static ssize_t rpi_uart_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    size_t i = 0;
    char tmp;

    while (i < count) {
        if (uart_readl(UART_FR) & UART_FR_RXFE) {
            if (i == 0)
                break;
            else
                return i;
        }

        tmp = (char)(uart_readl(UART_DR) & 0xFF);
        blink_led(); // Blink on each received byte
        if (copy_to_user(buf + i, &tmp, 1))
            return -EFAULT;
        i++;
    }

    return i;
}

static ssize_t rpi_uart_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    size_t i = 0;
    char tmp;

    while (i < count) {
        if (uart_readl(UART_FR) & UART_FR_TXFF) {
            if (i == 0)
                break;
            else
                return i;
        }

        if (copy_from_user(&tmp, buf + i, 1))
            return -EFAULT;

        uart_writel(tmp, UART_DR);
        while (uart_readl(UART_FR) & UART_FR_BUSY)
            cpu_relax();
        i++;
    }

    return i;
}

static struct file_operations rpi_uart_fops = {
    .owner = THIS_MODULE,
    .open = rpi_uart_open,
    .release = rpi_uart_release,
    .read = rpi_uart_read,
    .write = rpi_uart_write,
};

static int __init rpi_uart_module_init(void)
{
    int ret;

    uart_dev = kzalloc(sizeof(struct rpi_uart_dev), GFP_KERNEL);
    if (!uart_dev)
        return -ENOMEM;

    uart_dev->regs = ioremap(UART_BASE, 0x100);
    if (!uart_dev->regs) {
        pr_err("Failed to ioremap UART registers\n");
        kfree(uart_dev);
        return -ENOMEM;
    }

    ret = alloc_chrdev_region(&uart_dev->dev_no, 0, 1, DEVICE_NAME);
    if (ret)
        return ret;

    cdev_init(&uart_dev->cdev, &rpi_uart_fops);
    uart_dev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&uart_dev->cdev, uart_dev->dev_no, 1);
    if (ret)
        goto err_cdev;

    uart_dev->class = class_create(DEVICE_NAME);
    if (IS_ERR(uart_dev->class)) {
        ret = PTR_ERR(uart_dev->class);
        goto err_class;
    }

    uart_dev->device = device_create(uart_dev->class, NULL, uart_dev->dev_no, NULL, DEVICE_NAME);
    if (IS_ERR(uart_dev->device)) {
        ret = PTR_ERR(uart_dev->device);
        goto err_dev;
    }

    rpi_uart_hw_init();

    ret = gpio_request(LED_GPIO, "LED18");
    if (ret) {
        pr_err("LED GPIO %d request failed\n", LED_GPIO);
        goto err_gpio;
    }

    ret = gpio_direction_output(LED_GPIO, 0);
    if (ret) {
        pr_err("LED GPIO direction set failed\n");
        gpio_free(LED_GPIO);
        goto err_gpio;
    }

    pr_info("RPi UART driver from TechDhaba loaded successfully\n");
    return 0;

err_gpio:
    device_destroy(uart_dev->class, uart_dev->dev_no);
err_dev:
    class_destroy(uart_dev->class);
err_class:
    cdev_del(&uart_dev->cdev);
err_cdev:
    unregister_chrdev_region(uart_dev->dev_no, 1);
    iounmap(uart_dev->regs);
    kfree(uart_dev);
    return ret;
}

static void __exit rpi_uart_module_exit(void)
{
    gpio_set_value(LED_GPIO, 0);
    gpio_free(LED_GPIO);
    device_destroy(uart_dev->class, uart_dev->dev_no);
    class_destroy(uart_dev->class);
    cdev_del(&uart_dev->cdev);
    unregister_chrdev_region(uart_dev->dev_no, 1);
    iounmap(uart_dev->regs);
    kfree(uart_dev);
    pr_info("RPi UART driver from TechDhaba unloaded\n");
}

module_init(rpi_uart_module_init);
module_exit(rpi_uart_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ARYA");
MODULE_DESCRIPTION("Custom UART driver for Raspberry Pi 4B with GPIO18 LED blink");

