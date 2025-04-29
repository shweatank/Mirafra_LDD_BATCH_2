#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "keyboard_device"
#define CLASS_NAME  "keyboard_class"
#define KEYBOARD_IRQ 1  // IRQ for the PS/2 keyboard

static int    major_number;
static struct class*  my_class  = NULL;
static struct device* my_device = NULL;
static int    counter = 0;

// IRQ handler: increments and logs the counter
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    counter++;
    printk(KERN_INFO "[%s] IRQ fired. Counter = %d\n", DEVICE_NAME, counter);
    return IRQ_HANDLED;
}

// file_operations: open/release are no-ops beyond logging
static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Device opened\n", DEVICE_NAME);
    return 0;
}
static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[%s] Device closed\n", DEVICE_NAME);
    return 0;
}

// read() returns the current counter value as text
static ssize_t device_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    char message[32];
    int  msg_len = snprintf(message, sizeof(message), "%d\n", counter);

    if (*offset >= msg_len)
        return 0;  // EOF

    if (len > msg_len - *offset)
        len = msg_len - *offset;

    if (copy_to_user(buf, message + *offset, len))
        return -EFAULT;

    *offset += len;
    return len;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = device_open,
    .release = device_release,
    .read    = device_read,
};

static int __init kmod_init(void)
{
    int ret;

    printk(KERN_INFO "[%s] Initializing\n", DEVICE_NAME);

    // 1) register char device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "[%s] register_chrdev failed: %d\n", DEVICE_NAME, major_number);
        return major_number;
    }

    // 2) create device class
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "[%s] class_create failed\n", DEVICE_NAME);
        return PTR_ERR(my_class);
    }

    // 3) create /dev entry
    my_device = device_create(my_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "[%s] device_create failed\n", DEVICE_NAME);
        return PTR_ERR(my_device);
    }

    // 4) request the keyboard IRQ
    ret = request_irq(KEYBOARD_IRQ,
                      keyboard_interrupt_handler,
                      IRQF_SHARED,
                      "keyboard_interrupt_ops",
                      (void *)keyboard_interrupt_handler);
    if (ret) {
        device_destroy(my_class, MKDEV(major_number, 0));
        class_destroy(my_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ERR "[%s] request_irq failed: %d\n", DEVICE_NAME, ret);
        return ret;
    }

    printk(KERN_INFO "[%s] Module loaded. /dev/%s (major %d)\n",
           DEVICE_NAME, DEVICE_NAME, major_number);
    return 0;
}

static void __exit kmod_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)keyboard_interrupt_handler);
    device_destroy(my_class, MKDEV(major_number, 0));
    class_destroy(my_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "[%s] Module unloaded\n", DEVICE_NAME);
}

module_init(kmod_init);
module_exit(kmod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard interrupt counter with /dev interface");
