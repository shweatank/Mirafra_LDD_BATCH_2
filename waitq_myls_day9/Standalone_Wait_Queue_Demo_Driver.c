#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME "my_waitq_dev"
#define CLASS_NAME  "my_waitq_class"

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;

// Wait queue and flag
static wait_queue_head_t my_wq;
static int data_ready = 0;

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    int dummy_data = 1234;

    pr_info("Read called: waiting for data_ready...\n");

    // Sleep until data_ready becomes non-zero
    wait_event_interruptible(my_wq, data_ready != 0);

    // Copy dummy data to user
    if (copy_to_user(buf, &dummy_data, sizeof(dummy_data)))
        return -EFAULT;

    pr_info("Read returning after wakeup, data sent = %d\n", dummy_data);

    // Reset flag
    data_ready = 0;

    return sizeof(dummy_data);
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
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

static int __init my_init(void) {
    // Allocate device number
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0)
        return -1;

    // Init cdev
    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev_num, 1) < 0)
        goto unregister;

    // Create class and device node
    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(my_class))
        goto del_cdev;

    if (IS_ERR(device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME)))
        goto destroy_class;

    // Init wait queue
    init_waitqueue_head(&my_wq);

    pr_info("Wait queue demo driver loaded.\n");
    return 0;

destroy_class:
    class_destroy(my_class);
del_cdev:
    cdev_del(&my_cdev);
unregister:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

static void __exit my_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Wait queue demo driver unloaded.\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Standalone Wait Queue Demo Driver");
