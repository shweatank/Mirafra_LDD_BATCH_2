#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/sched.h>

#define DEVICE_NAME "waitqueue_dev"
#define CLASS_NAME "waitqueue_class"

static dev_t dev_number;
static struct cdev cdev;
static struct class *dev_class;

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int condition_flag = 0;  // Shared condition for the wait queue
static DEFINE_MUTEX(lock);

static int dev_open(struct inode *inode, struct file *file) {
    pr_info("Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int ret;

    pr_info("Read called, waiting for condition...\n");

    // Wait for the condition to be true
    ret = wait_event_interruptible(wq, condition_flag != 0);
    if (ret == 0) {
        mutex_lock(&lock);
        condition_flag = 0;  // Reset the condition
        mutex_unlock(&lock);
        pr_info("Condition met, returning from read\n");
    }

    return 0;
}

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    pr_info("Write called, setting condition and waking up readers\n");

    // Change the condition and wake up the waiting processes
    mutex_lock(&lock);
    condition_flag = 1;  // Set the condition flag
    mutex_unlock(&lock);

    wake_up_interruptible(&wq);  // Wake up any waiting processes
    return len;
}

static int dev_release(struct inode *inode, struct file *file) {
    pr_info("Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init waitqueue_driver_init(void) {
    if (alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME) < 0) {
        pr_err("Failed to allocate device number\n");
        return -1;
    }

    cdev_init(&cdev, &fops);
    if (cdev_add(&cdev, dev_number, 1) < 0) {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    dev_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dev_class)) {
        pr_err("Failed to create device class\n");
        cdev_del(&cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(dev_class);
    }

    if (IS_ERR(device_create(dev_class, NULL, dev_number, NULL, DEVICE_NAME))) {
        pr_err("Failed to create device\n");
        class_destroy(dev_class);
        cdev_del(&cdev);
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    pr_info("Wait Queue Device Initialized\n");
    return 0;
}

static void __exit waitqueue_driver_exit(void) {
    device_destroy(dev_class, dev_number);
    class_destroy(dev_class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev_number, 1);
    pr_info("Wait Queue Device Unloaded\n");
}

module_init(waitqueue_driver_init);
module_exit(waitqueue_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Character Device with Wait Queue");
MODULE_VERSION("1.0");

