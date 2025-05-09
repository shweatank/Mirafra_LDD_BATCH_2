#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME "my_waitq_dev"

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

// Wait queue and flag
static wait_queue_head_t my_wq;
static int data_ready = 0;

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    int dummy_data = 1234;

    pr_info("Read called: waiting for data_ready...\n");

    // Sleep until data_ready is set
    wait_event_interruptible(my_wq, data_ready != 0);

    // Copy dummy data to user space
    if (copy_to_user(buf, &dummy_data, sizeof(dummy_data)))
        return -EFAULT;

    pr_info("Read returning after wakeup, data sent = %d\n", dummy_data);

    // Reset flag
    data_ready = 0;

    return sizeof(dummy_data);
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("Write called: waking up reader...\n");

    // Set flag and wake up wait queue
    data_ready = 1;
    wake_up_interruptible(&my_wq);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = my_read,
    .write = my_write,
};

static int __init my_init(void)
{
    int ret;

    // Allocate major and minor number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    // Init cdev
    cdev_init(&my_cdev, &fops);
    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0)
        goto unregister;

    // ✅ Create class (correct API for 6.8 kernel)
    my_class = class_create("my_waitq_class");
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        goto del_cdev;
    }

    // Create device node in /dev
    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        ret = PTR_ERR(my_device);
        goto destroy_class;
    }

    // Initialize wait queue
    init_waitqueue_head(&my_wq);

    pr_info("Wait queue driver loaded.\n");
    return 0;

destroy_class:
    class_destroy(my_class);
del_cdev:
    cdev_del(&my_cdev);
unregister:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("Wait queue driver unloaded.\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tech Dhaba");
MODULE_DESCRIPTION("Standalone Wait Queue Driver (Correct for Kernel 6.8)");
