#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define DEVICE_NAME "dma_i2c_dev"
#define CLASS_NAME "dma_i2c_class"
#define BUFFER_SIZE 256

static int major;
static struct class* dma_class = NULL;
static struct device* dma_device = NULL;
static struct task_struct *dma_thread = NULL;

static char *dma_buffer;
static int dma_requested = 0;
static int dma_data_ready = 0;
static DECLARE_WAIT_QUEUE_HEAD(dma_wait);

// ==========================
// Simulated DMA Thread
// ==========================
static int dma_thread_fn(void *data)
{
    while (!kthread_should_stop()) {
        wait_event_interruptible(dma_wait, dma_requested || kthread_should_stop());

        if (kthread_should_stop())
            break;

        // Simulate DMA transfer delay
        msleep(500);

        // Simulate I2C read via DMA into buffer
        snprintf(dma_buffer, BUFFER_SIZE, "DMA I2C Simulated Data at jiffies=%lu\n", jiffies);
        dma_data_ready = 1;
        dma_requested = 0;

        printk(KERN_INFO "dma_i2c: DMA transfer completed.\n");
    }
    return 0;
}

// ==========================
// File Operations
// ==========================
static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "dma_i2c: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "dma_i2c: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *user_buf, size_t len, loff_t *offset)
{
    if (*offset > 0 || !dma_data_ready)
        return 0;

    if (copy_to_user(user_buf, dma_buffer, strlen(dma_buffer)))
        return -EFAULT;

    dma_data_ready = 0;
    *offset = strlen(dma_buffer);
    return *offset;
}

static ssize_t dev_write(struct file *filep, const char __user *user_buf, size_t len, loff_t *offset)
{
    char user_data[32];

    if (len > 31) len = 31;

    if (copy_from_user(user_data, user_buf, len))
        return -EFAULT;

    user_data[len] = '\0';

    if (strncmp(user_data, "dma_read", 8) == 0) {
        dma_requested = 1;
        wake_up_interruptible(&dma_wait);
        printk(KERN_INFO "dma_i2c: DMA read requested.\n");
    }

    return len;
}

static struct file_operations fops =
{
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

// ==========================
// Module Init/Exit
// ==========================
static int __init dma_driver_init(void)
{
    printk(KERN_INFO "dma_i2c: Initializing driver...\n");

    dma_buffer = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!dma_buffer) {
        printk(KERN_ALERT "dma_i2c: Failed to allocate DMA buffer\n");
        return -ENOMEM;
    }

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        kfree(dma_buffer);
        printk(KERN_ALERT "dma_i2c: Failed to register major number\n");
        return major;
    }

    dma_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dma_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        kfree(dma_buffer);
        return PTR_ERR(dma_class);
    }

    dma_device = device_create(dma_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(dma_device)) {
        class_destroy(dma_class);
        unregister_chrdev(major, DEVICE_NAME);
        kfree(dma_buffer);
        return PTR_ERR(dma_device);
    }

    dma_thread = kthread_run(dma_thread_fn, NULL, "dma_i2c_thread");
    if (IS_ERR(dma_thread)) {
        device_destroy(dma_class, MKDEV(major, 0));
        class_destroy(dma_class);
        unregister_chrdev(major, DEVICE_NAME);
        kfree(dma_buffer);
        printk(KERN_ALERT "dma_i2c: Failed to create kernel thread\n");
        return PTR_ERR(dma_thread);
    }

    printk(KERN_INFO "dma_i2c: Driver loaded successfully.\n");
    return 0;
}

static void __exit dma_driver_exit(void)
{
    if (dma_thread)
        kthread_stop(dma_thread);

    device_destroy(dma_class, MKDEV(major, 0));
    class_destroy(dma_class);
    unregister_chrdev(major, DEVICE_NAME);
    kfree(dma_buffer);

    printk(KERN_INFO "dma_i2c: Driver unloaded.\n");
}

module_init(dma_driver_init);
module_exit(dma_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simulated I2C DMA Device Driver using kthread");
MODULE_VERSION("1.0");
