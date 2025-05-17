#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>

static dev_t dev;
static struct cdev cdev;
static struct class *class;
static struct device *device;
static wait_queue_head_t wq;
static int data_ready = 0;

static int dev_open(struct inode *inode, struct file *file) {
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    pr_info("Reader: Waiting for data...\n");
    wait_event_interruptible(wq, data_ready);
    pr_info("Reader: Data ready, waking up!\n");
    data_ready = 0; // Reset for next read
    return 0; // Simplified: no data transfer
}

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    pr_info("Writer: Setting data ready\n");
    data_ready = 1;
    wake_up_interruptible(&wq);
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init waitqueue_init(void) {
    // Allocate device number
    if (alloc_chrdev_region(&dev, 0, 1, "waitqueue_dev") < 0) {
        pr_err("Failed to allocate device number\n");
        return -1;
    }

    // Initialize cdev
    cdev_init(&cdev, &fops);
    if (cdev_add(&cdev, dev, 1) < 0) {
        pr_err("Failed to add cdev\n");
        goto cleanup_chrdev;
    }

    // Create device class
    class = class_create(THIS_MODULE, "waitqueue_class");
    if (IS_ERR(class)) {
        pr_err("Failed to create class\n");
        goto cleanup_cdev;
    }

    // Create device
    device = device_create(class, NULL, dev, NULL, "waitqueue_dev");
    if (IS_ERR(device)) {
        pr_err("Failed to create device\n");
        goto cleanup_class;
    }

    // Initialize wait queue
    init_waitqueue_head(&wq);
    pr_info("Waitqueue module loaded\n");
    return 0;

cleanup_class:
    class_destroy(class);
cleanup_cdev:
    cdev_del(&cdev);
cleanup_chrdev:
    unregister_chrdev_region(dev, 1);
    return -1;
}

static void __exit waitqueue_exit(void) {
    device_destroy(class, dev);
    class_destroy(class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Waitqueue module unloaded\n");
}

module_init(waitqueue_init);
module_exit(waitqueue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example");
MODULE_DESCRIPTION("Wait queue example module");
