#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "charcalc"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static char reverse[BUF_LEN];
static int result;

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "charcalc: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "charcalc: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int msg_len;
    if (*offset > 0) return 0; // prevent multiple reads

    msg_len = strlen(reverse);
    if (msg_len == 0)
        return 0;

    if (copy_to_user(buffer, reverse, msg_len))
        return -EFAULT;

    *offset += msg_len;
    return msg_len;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';
    
    int l = strlen(message);
    if (message[l - 1] == '\n') {
        message[l - 1] = '\0';
        l--;
    }
    for (int i = 0; i < l; i++) {
        reverse[i] = message[l - i - 1];
    }

    reverse[l + 1] = '\0';  // Proper null-termination

    printk(KERN_INFO "charcalc: Original: %s, Reversed: %s\n", message, reverse);

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
        printk(KERN_ALERT "charcalc failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "charcalc: registered with major number %d\n", major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "charcalc: unregistered\n");
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple character device that performs + and -");
