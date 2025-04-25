#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define CLASS_NAME  "mycharclass"

MODULE_LICENSE("GPL");

static int major;
static int input1 = 42;
static int input2 = 84;
static char message[256];
static short message_len;

static struct class*  charClass  = NULL;
static struct device* charDevice = NULL;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .release = dev_release,
};

static int __init mychardev_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major;
    }

    charClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(charClass)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(charClass);
    }

    charDevice = device_create(charClass, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(charDevice)) {
        class_destroy(charClass);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(charDevice);
    }

    printk(KERN_INFO "mychardev: registered correctly with major number %d\n", major);
    return 0;
}

static void __exit mychardev_exit(void) {
    device_destroy(charClass, MKDEV(major, 0));
    class_unregister(charClass);
    class_destroy(charClass);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "mychardev: Goodbye!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
    sprintf(message, "Input1: %d, Input2: %d\n", input1, input2);
    message_len = strlen(message);
    printk(KERN_INFO "mychardev: Device has been opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    int error_count = 0;

    if (*offset >= message_len)
        return 0;

    error_count = copy_to_user(buffer, message + *offset, message_len - *offset);
    if (error_count == 0){
        int ret = message_len - *offset;
        *offset = message_len;
        printk(KERN_INFO "mychardev: Sent %d characters to the user\n", ret);
        return ret;
    } else {
        printk(KERN_INFO "mychardev: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static int dev_release(struct inode *inodep, struct file *filep){
    printk(KERN_INFO "mychardev: Device successfully closed\n");
    return 0;
}

module_init(mychardev_init);
module_exit(mychardev_exit);

