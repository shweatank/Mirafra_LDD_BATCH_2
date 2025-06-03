#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "kthread_device"
#define CLASS_NAME "kthread_class"

static struct task_struct *thread_st;
static int major;
static struct class* kthread_class = NULL;
static struct device* kthread_device = NULL;
static int thread_should_run = 1;

// =============================
// Kernel Thread Function
// =============================
static int thread_fn(void *unused)
{
    printk(KERN_INFO "kthread_device: Kernel thread started.\n");

    while (!kthread_should_stop()) {
        if (thread_should_run)
            printk(KERN_INFO "kthread_device: Running in kernel thread...\n");

        ssleep(1);
    }

    printk(KERN_INFO "kthread_device: Kernel thread stopped.\n");
    return 0;
}

// =============================
// File Operations
// =============================
static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "kthread_device: Device opened.\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "kthread_device: Device closed.\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    char *msg = "Hello from kernel thread driver\n";
    size_t msg_len = strlen(msg);
    
    if (*offset >= msg_len)
        return 0;

    if (copy_to_user(buffer, msg, msg_len))
        return -EFAULT;

    *offset += msg_len;
    return msg_len;
}

static struct file_operations fops =
{
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
};

// =============================
// Module Init & Exit
// =============================
static int __init kthread_driver_init(void)
{
    printk(KERN_INFO "kthread_device: Initializing...\n");

    // Register char device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "kthread_device: Failed to register a major number\n");
        return major;
    }

    kthread_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(kthread_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "kthread_device: Failed to register device class\n");
        return PTR_ERR(kthread_class);
    }

    kthread_device = device_create(kthread_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(kthread_device)) {
        class_destroy(kthread_class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "kthread_device: Failed to create the device\n");
        return PTR_ERR(kthread_device);
    }

    // Start kernel thread
    thread_st = kthread_run(thread_fn, NULL, "kthread_driver_thread");
    if (IS_ERR(thread_st)) {
        device_destroy(kthread_class, MKDEV(major, 0));
        class_destroy(kthread_class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "kthread_device: Failed to create thread\n");
        return PTR_ERR(thread_st);
    }

    printk(KERN_INFO "kthread_device: Module loaded successfully.\n");
    return 0;
}

static void __exit kthread_driver_exit(void)
{
    printk(KERN_INFO "kthread_device: Exiting module...\n");

    if (thread_st)
        kthread_stop(thread_st);

    device_destroy(kthread_class, MKDEV(major, 0));
    class_destroy(kthread_class);
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "kthread_device: Module unloaded.\n");
}

module_init(kthread_driver_init);
module_exit(kthread_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple Device Driver using kthread");
MODULE_VERSION("1.0");
