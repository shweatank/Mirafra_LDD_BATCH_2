#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define BUF_LEN 1024

static int major;
static char kernel_buffer[BUF_LEN];

static ssize_t dev_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
    if (len > BUF_LEN - 1)
        len = BUF_LEN - 1;

    if (copy_from_user(kernel_buffer, buffer, len)) {
        return -EFAULT;
    }

    kernel_buffer[len] = '\0';
    printk(KERN_INFO "User Input: %s\n", kernel_buffer);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = dev_write,
};

static int __init chardev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", major);
        return major;
    }
    printk(KERN_INFO "Char device registered with major number %d\n", major);
    return 0;
}

static void __exit chardev_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Char device unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_DESCRIPTION("Simple Linux char driver that logs user input to kernel log");
