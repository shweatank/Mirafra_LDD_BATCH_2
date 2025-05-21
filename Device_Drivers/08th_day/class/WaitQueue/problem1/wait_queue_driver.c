#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/cdev.h>

#define DEVICE_NAME "waitqueue_dev"
#define CLASS_NAME "waitqueue_class"


static int major;
static struct class *waitqueue_class = NULL;
static struct device *waitqueue_device = NULL;
static struct cdev waitqueue_cdev;

static wait_queue_head_t wq;
static int condition_flag = 0;
static DEFINE_SPINLOCK(flag_lock);

static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "WaitQueueDev: Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    int ret;

    printk(KERN_INFO "WaitQueueDev: Read attempt\n");

    /* Wait until condition_flag is set */
    ret = wait_event_interruptible_timeout(wq, condition_flag == 1, msecs_to_jiffies(5000));
    if (ret == 0) {
        printk(KERN_INFO "WaitQueueDev: Timeout occurred\n");
        return -ETIMEDOUT;
    } else if (ret < 0) {
        printk(KERN_INFO "WaitQueueDev: Wait interrupted\n");
        return ret;
    }

    /* Condition met, proceed with read */
    if (len < sizeof(int))
        return -EINVAL;

    spin_lock(&flag_lock);
    if (copy_to_user(buf, &condition_flag, sizeof(int))) {
        spin_unlock(&flag_lock);
        return -EFAULT;
    }
    /* Reset flag after read */
    condition_flag = 0;
    spin_unlock(&flag_lock);

    printk(KERN_INFO "WaitQueueDev: Read successful, flag=%d\n", condition_flag);
    return sizeof(int);
}

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    int value;

    if (len < sizeof(int))
        return -EINVAL;

    if (copy_from_user(&value, buf, sizeof(int)))
        return -EFAULT;

    spin_lock(&flag_lock);
    condition_flag = value;
    spin_unlock(&flag_lock);

    printk(KERN_INFO "WaitQueueDev: Write set flag=%d\n", condition_flag);

    /* Wake up all waiting processes */
    wake_up_interruptible(&wq);

    return sizeof(int);
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "WaitQueueDev: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init waitqueue_init(void)
{
    dev_t dev;

    /* Initialize wait queue */
    init_waitqueue_head(&wq);

    /* Allocate major number */
    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ERR "WaitQueueDev: Failed to allocate major number\n");
        return -1;
    }
    major = MAJOR(dev);

    /* Initialize cdev */
    cdev_init(&waitqueue_cdev, &fops);
    if (cdev_add(&waitqueue_cdev, dev, 1) < 0) {
        printk(KERN_ERR "WaitQueueDev: Failed to add cdev\n");
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    /* Create device class */
    waitqueue_class = class_create(CLASS_NAME);
    if (IS_ERR(waitqueue_class)) {
        printk(KERN_ERR "WaitQueueDev: Failed to create class\n");
        cdev_del(&waitqueue_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(waitqueue_class);
    }

    /* Create device */
    waitqueue_device = device_create(waitqueue_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(waitqueue_device)) {
        printk(KERN_ERR "WaitQueueDev: Failed to create device\n");
        class_destroy(waitqueue_class);
        cdev_del(&waitqueue_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(waitqueue_device);
    }

    printk(KERN_INFO "WaitQueueDev: Initialized with major number %d\n", major);
    return 0;
}

static void __exit waitqueue_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    device_destroy(waitqueue_class, dev);
    class_destroy(waitqueue_class);
    cdev_del(&waitqueue_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "WaitQueueDev: Module unloaded\n");
}

module_init(waitqueue_init);
module_exit(waitqueue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grok");
MODULE_DESCRIPTION("Character device driver with wait queue");
