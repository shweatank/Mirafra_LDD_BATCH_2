#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/sched.h>

#define DEVICE_NAME "waitq_dev"
#define CLASS_NAME "waitq_class"
#define TIMEOUT_MS 5000 // Optional: timeout in milliseconds

static int major;
static struct class* waitq_class = NULL;
static struct device* waitq_device = NULL;
static struct cdev waitq_cdev;

static DECLARE_WAIT_QUEUE_HEAD(waitq);
static int condition_flag = 0;
static DEFINE_MUTEX(waitq_mutex);

static int waitq_open(struct inode *inode, struct file *file) {
    pr_info("waitq_dev: Device opened\n");
    return 0;
}

static int waitq_release(struct inode *inode, struct file *file) {
    pr_info("waitq_dev: Device closed\n");
    return 0;
}

static ssize_t waitq_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int ret;

    pr_info("waitq_dev: Read called. Sleeping if condition not met.\n");

    // Wait with timeout (optional)
    ret = wait_event_interruptible_timeout(waitq, condition_flag != 0,
                                           msecs_to_jiffies(TIMEOUT_MS));

    if (ret == 0) {
        pr_info("waitq_dev: Timeout occurred\n");
        return -EAGAIN;
    } else if (ret < 0) {
        pr_info("waitq_dev: Interrupted by signal\n");
        return -ERESTARTSYS;
    }

    mutex_lock(&waitq_mutex);
    condition_flag = 0; // Reset the condition after reading
    mutex_unlock(&waitq_mutex);

    pr_info("waitq_dev: Woken up, returning to user.\n");

    return 0;
}

static ssize_t waitq_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    pr_info("waitq_dev: Write called. Waking up readers.\n");

    mutex_lock(&waitq_mutex);
    condition_flag = 1;
    mutex_unlock(&waitq_mutex);

    wake_up_interruptible(&waitq);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = waitq_open,
    .release = waitq_release,
    .read = waitq_read,
    .write = waitq_write,
};

static int __init waitq_init(void) {
    dev_t dev;
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("waitq_dev: Failed to allocate major number\n");
        return ret;
    }

    major = MAJOR(dev);
    cdev_init(&waitq_cdev, &fops);
    waitq_cdev.owner = THIS_MODULE;

    ret = cdev_add(&waitq_cdev, dev, 1);
    if (ret < 0) {
        pr_err("waitq_dev: Failed to add cdev\n");
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    waitq_class = class_create(CLASS_NAME);
    if (IS_ERR(waitq_class)) {
        pr_err("waitq_dev: Failed to create class\n");
        cdev_del(&waitq_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(waitq_class);
    }

    waitq_device = device_create(waitq_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(waitq_device)) {
        pr_err("waitq_dev: Failed to create device\n");
        class_destroy(waitq_class);
        cdev_del(&waitq_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(waitq_device);
    }

    mutex_init(&waitq_mutex);
    pr_info("waitq_dev: Module loaded successfully. Major = %d\n", major);
    return 0;
}

static void __exit waitq_exit(void) {
    device_destroy(waitq_class, MKDEV(major, 0));
    class_destroy(waitq_class);
    cdev_del(&waitq_cdev);
    unregister_chrdev_region(MKDEV(major, 0), 1);
    mutex_destroy(&waitq_mutex);
    pr_info("waitq_dev: Module unloaded\n");
}

module_init(waitq_init);
module_exit(waitq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Character Device with Wait Queue");
MODULE_VERSION("1.0");

