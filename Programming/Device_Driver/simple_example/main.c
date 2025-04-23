#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define MAJOR_NUM 240

static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "simple_char_dev: Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "simple_char_dev: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
};

static int __init simple_char_init(void) {
    int result = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (result < 0) {
        printk(KERN_ALERT "simple_char_dev: Failed to register device\n");
        return result;
    }
    printk(KERN_INFO "simple_char_dev: Registered with major number %d\n", MAJOR_NUM);
    return 0;
}

static void __exit simple_char_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "simple_char_dev: Unregistered device\n");
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A very basic Linux character device driver");
