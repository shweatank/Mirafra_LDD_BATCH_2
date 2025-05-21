#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "simple_calculator"
#define BUF_LEN 100

#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, char *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, char *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant");
MODULE_DESCRIPTION("Kernel Calculator using IOCTL and kmalloc");
MODULE_VERSION("1.0");

static int major;
static char *message;
static char *reversed;

// === Device Open/Close ===
static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "calcdata: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "calcdata: Device closed\n");
    return 0;
}

// === IOCTL Handler ===
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    size_t len, i;

    switch (cmd) {
        case IOCTL_SET_DATA:
            // Allocate buffer
            message = kmalloc(BUF_LEN, GFP_KERNEL);
            reversed = kmalloc(BUF_LEN, GFP_KERNEL);

            if (!message || !reversed)
                return -ENOMEM;

            // Copy data from user
            if (copy_from_user(message, (char __user *)arg, BUF_LEN)) {
                return -EFAULT;
            }

            message[BUF_LEN - 1] = '\0';
            len = strnlen(message, BUF_LEN);

            // Reverse the string
            for (i = 0; i < len; i++) {
                reversed[i] = message[len - i - 1];
            }
            reversed[len] = '\0';

            printk(KERN_INFO "calcdata: Reversed = %s\n", reversed);
            break;

        case IOCTL_GET_RESULT:
            if (copy_to_user((char __user *)arg, reversed, BUF_LEN)) {
                return -EFAULT;
            }
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

// === File Operations Struct ===
static struct file_operations fops = {
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

// === Init/Exit ===
static int __init simple_calculator_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "calcdata: Failed to register major number\n");
        return major;
    }
    printk(KERN_INFO "calcdata: Registered with major number %d\n", major);
    return 0;
}

static void __exit simple_calculator_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    if (message)
        kfree(message);
    if (reversed)
        kfree(reversed);

    printk(KERN_INFO "calcdata: Unregistered module\n");
}

module_init(simple_calculator_init);
module_exit(simple_calculator_exit);

