#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "simple_calculator"
#define BUF_LEN 100

#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, char *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, char *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("vaneesha");
MODULE_DESCRIPTION("Kernel Calculator using IOCTL (no kmalloc)");
MODULE_VERSION("1.0");

static int major;
static char message[BUF_LEN];
static char reversed[BUF_LEN];

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
            if (copy_from_user(message, (char __user *)arg, BUF_LEN))
                return -EFAULT;

            message[BUF_LEN - 1] = '\0'; // ensure null-termination
            len = strnlen(message, BUF_LEN);

            for (i = 0; i < len; i++) {
                reversed[i] = message[len - i - 1];
            }
            reversed[len] = '\0';

            printk(KERN_INFO "calcdata: Input = %s, Reversed = %s\n", message, reversed);
            break;

        case IOCTL_GET_RESULT:
            if (copy_to_user((char __user *)arg, reversed, BUF_LEN))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

// === File Operations ===
static struct file_operations fops = {
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

// === Module Init/Exit ===
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
    printk(KERN_INFO "calcdata: Module unloaded\n");
}

module_init(simple_calculator_init);
module_exit(simple_calculator_exit);

