#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#define DEVICE_NAME "my_char_dev"
#define CLASS_NAME  "my_char_class"

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;

static struct workqueue_struct *my_wq;

// Define work structure
struct my_work_t {
    struct work_struct work;
    int data;
};

static void my_work_handler(struct work_struct *work) {
    struct my_work_t *my_work = container_of(work, struct my_work_t, work);
    pr_info("Workqueue: Processing data = %d\n", my_work->data);

    // Simulate a delay
    msleep(1000);

    kfree(my_work);
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    struct my_work_t *work;
    int value;

    if (copy_from_user(&value, buf, sizeof(value)))
        return -EFAULT;

    pr_info("Write called with value: %d\n", value);

    // Allocate work structure
    work = kmalloc(sizeof(struct my_work_t), GFP_KERNEL);
    if (!work)
        return -ENOMEM;

    // Initialize work
    INIT_WORK(&work->work, my_work_handler);
    work->data = value;

    // Queue work
    queue_work(my_wq, &work->work);

    return sizeof(value);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = my_write,
};

static int __init my_init(void) {
    // Allocate char device numbers
    if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0)
        return -1;

    // Init cdev
    cdev_init(&my_cdev, &fops);
    if (cdev_add(&my_cdev, dev_num, 1) < 0)
        goto unregister;

    // Create class and device
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class))
        goto del_cdev;

    if (IS_ERR(device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME)))
        goto destroy_class;

    // Create workqueue
    my_wq = create_singlethread_workqueue("my_wq");
    if (!my_wq)
        goto destroy_device;

    pr_info("Module loaded. Device ready.\n");
    return 0;

destroy_device:
    device_destroy(my_class, dev_num);
destroy_class:
    class_destroy(my_class);
del_cdev:
    cdev_del(&my_cdev);
unregister:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

static void __exit my_exit(void) {
    // Cleanup
    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);

    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("Module unloaded.\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("Simple Character Device + Workqueue");
