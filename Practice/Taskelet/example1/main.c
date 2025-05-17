#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>

static dev_t dev;
static struct cdev cdev;
static struct class *class;
static struct device *device;
static struct tasklet_struct my_tasklet;

static void tasklet_function(unsigned long data) {
    pr_info("Tasklet running in softirq context, data: %ld\n", data);
}

static int dev_open(struct inode *inode, struct file *file) {
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char msg[] = "Tasklet demo: Write to schedule tasklet\n";
    size_t msg_len = strlen(msg) + 1;

    if (*off >= msg_len)
        return 0;

    if (len > msg_len - *off)
        len = msg_len - *off;

    if (copy_to_user(buf, msg + *off, len))
        return -EFAULT;

    *off += len;
    return len;
}

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    pr_info("Scheduling tasklet\n");
    tasklet_schedule(&my_tasklet);
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init tasklet_init(void) {
    // Initialize tasklet
    tasklet_init(&my_tasklet, tasklet_function, 42); // Pass 42 as example data

    // Allocate device number
    if (alloc_chrdev_region(&dev, 0, 1, "tasklet_dev") < 0) {
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
    class = class_create(THIS_MODULE, "tasklet_class");
    if (IS_ERR(class)) {
        pr_err("Failed to create class\n");
        goto cleanup_cdev;
    }

    // Create device
    device = device_create(class, NULL, dev, NULL, "tasklet_dev");
    if (IS_ERR(device)) {
        pr_err("Failed to create device\n");
        goto cleanup_class;
    }

    pr_info("Tasklet module loaded\n");
    return 0;

cleanup_class:
    class_destroy(class);
cleanup_cdev:
    cdev_del(&cdev);
cleanup_chrdev:
    unregister_chrdev_region(dev, 1);
    return -1;
}

static void __exit tasklet_exit(void) {
    tasklet_kill(&my_tasklet); // Ensure tasklet is stopped
    device_destroy(class, dev);
    class_destroy(class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Tasklet module unloaded\n");
}

module_init(tasklet_init);
module_exit(tasklet_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example");
MODULE_DESCRIPTION("Tasklet example module");
