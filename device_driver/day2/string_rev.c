#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define DEVICE_NAME "calcdata"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static char reversed[BUF_LEN];

// === File Operation Functions ===
static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "calcdata: device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "calcdata: device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char result_msg[BUF_LEN];
    int msg_len = snprintf(result_msg, BUF_LEN, "Result: %s\n", reversed);
    int error_count = copy_to_user(buffer, result_msg, msg_len);

    return error_count == 0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    size_t i;
    if (len >= BUF_LEN)
        len = BUF_LEN - 1;  // Prevent overflow

    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';

    // Reverse the input
    for (i = 0; i < len; i++) {
        reversed[i] = message[len - i - 1];
    }
    reversed[len] = '\0';

    printk(KERN_INFO "calcdata: Reversed input = %s\n", reversed);

    return len;
}

// === File Operations Structure ===
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release
};

// === Module Init/Exit Functions ===
static int __init calcdata_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "calcdata: Failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "calcdata: Registered with major number %d\n", major);
    return 0;
}

static void __exit calcdata_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "calcdata: Unregistered module\n");
}

module_init(calcdata_init);
module_exit(calcdata_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple character device that reverses input strings");

