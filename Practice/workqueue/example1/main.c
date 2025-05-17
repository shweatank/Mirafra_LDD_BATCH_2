#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

static dev_t dev;
static struct cdev cdev;
static struct class *class;
static struct device *device;
static struct work_struct my_work;

static void work_function(struct work_struct *work) {
    pr_info("Workqueue: Running in process context (pid: %d)\n", current->pid);
    msleep(100); // Simulate blocking operation (safe in process context)
    pr_info("Workqueue: Work completed\n");
}

static int dev_open(struct inode *inode, struct file *file) {
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char msg[] = "Workqueue demo: Write to schedule work\n";
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
    pr_info("Scheduling work\n");
    schedule_work(&my_work);
    return len;
}

static int __init workqueue_init(void) {
    // Initialize work
    INIT_WORK(&my_work, work_function);

    // Allocate device number
    if (alloc_chrdev_region(&dev, 0, 1, "workqueue_dev") < 0) {
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
    class = class_create(THIS_MODULE, "workqueue_class");
    if (IS_ERR(class)) {
        pr_err("Failed to create class\n");
        goto cleanup_cdev;
    }

    // Create device
    device = device_create(class, NULL, dev, NULL, "workqueue_dev");
    if (IS_ERR(device)) {
        pr_err("Failed to create device\n");
        goto cleanup_class;
    }

    pr_info("Workqueue module loaded\n");
    return 0;

cleanup_class:
    class_destroy(class);
cleanup_cdev:
    cdev_del(&cdev);
cleanup_chrdev:
    unregister_chrdev_region(dev, 1);
    return -1;
}

static void __exit workqueue_exit(void) {
    cancel_work_sync(&my_work); // Ensure work is cancelled
    device_destroy(class, dev);
    class_destroy(class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Workqueue module unloaded\n");
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

module_init(workqueue_init);
module_exit(workqueue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example");
MODULE_DESCRIPTION("Workqueue example module");
