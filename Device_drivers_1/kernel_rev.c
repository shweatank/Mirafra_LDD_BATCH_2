#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "kernel_rev"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "kernel_rev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "kernel_rev: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) 
{
    char result_msg[BUF_LEN];
    int msg_len = snprintf(result_msg, BUF_LEN, "Result: %s\n", message);
    int error_count = copy_to_user(buffer, result_msg, msg_len);

    return error_count == 0 ? msg_len : -EFAULT;

}
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    int start, end,i;
    char temp;

    if (len >= BUF_LEN)
        len = BUF_LEN - 1;

    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';

    start = 0;
    while (message[i] != '\0')
        start++;

    end = start - 1;
    start = 0;

    while (start < end) {
        temp = message[start];
        message[start] = message[end];
        message[end] = temp;
        start++;
        end--;
    }

    printk(KERN_INFO "kernel_rev: Reversed string: %s\n", message);
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init charcalc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "kernel_rev failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "kernel_rev: registered with major number %d\n", major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "kernel_rev: unregistered\n");
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple character device that reverses a string");
