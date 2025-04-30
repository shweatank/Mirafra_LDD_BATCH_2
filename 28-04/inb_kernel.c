#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/uaccess.h>

#define IRQ_NUMBER 1
#define DEVICE "keyboard"

static int major;
char message[100];
int result = 0;
int a = 0, b = 0;

static irqreturn_t keyboard_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    scancode = inb(0x60);  // Read the keyboard scan code

    // Ignore key release events
    if (scancode & 0x80)
        return IRQ_HANDLED;

    printk(KERN_INFO "Keyboard IRQ: scancode = 0x%02x\n", scancode);

    if (scancode == 0x1e) {         // 'a' key pressed
        result = a + b;
        printk(KERN_INFO "ADD: %d + %d = %d\n", a, b, result);
    } else if (scancode == 0x30) {  // 'b' key pressed
        result = a - b;
        printk(KERN_INFO "SUB: %d - %d = %d\n", a, b, result);
    } else {
        result = a * b;
        printk(KERN_INFO "MUL: %d * %d = %d\n", a, b, result);
    }

    return IRQ_HANDLED;
}

static int dev_open(struct inode *inode, struct file *fp)
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *fp)
{
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *fp, char __user *buf, size_t len, loff_t *offset)
{
    char result_msg[100];
    int msg_len = snprintf(result_msg, sizeof(result_msg), "Result: %d\n", result);

    if (copy_to_user(buf, result_msg, msg_len))
        return -EFAULT;

    return msg_len;
}

static ssize_t dev_write(struct file *fp, const char __user *buf, size_t len, loff_t *offset)
{
    if (len >= sizeof(message)) len = sizeof(message) - 1;

    if (copy_from_user(message, buf, len))
        return -EFAULT;

    message[len] = '\0';

    if (sscanf(message, "%d %d", &a, &b) != 2) {
        printk(KERN_INFO "Invalid input format. Expected: <int> <int>\n");
        return -EINVAL;
    }

    printk(KERN_INFO "Received input: a = %d, b = %d\n", a, b);
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .write = dev_write,
    .read = dev_read,
};

static int __init hello_init(void)
{
    int ret;

    ret = request_irq(IRQ_NUMBER, keyboard_handler, IRQF_SHARED,
                      "keyboard_interrupt", (void *)keyboard_handler);
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ\n");
        return ret;
    }

    major = register_chrdev(0, DEVICE, &fops);
    if (major < 0) {
        free_irq(IRQ_NUMBER, (void *)keyboard_handler);
        printk(KERN_ERR "Failed to register char device\n");
        return major;
    }

    printk(KERN_INFO "Device registered with major number %d\n", major);
    return 0;
}

static void __exit hello_exit(void)
{
    free_irq(IRQ_NUMBER, (void *)keyboard_handler);
    unregister_chrdev(major, DEVICE);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Keyboard IRQ Calculation Driver");

