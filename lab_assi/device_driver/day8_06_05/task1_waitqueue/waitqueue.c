#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/device.h>

#define DEVICE_NAME "waitq_char"
#define CLASS_NAME "waitq_class"

static int major;
static struct class* waitq_class = NULL;
static struct device* waitq_device = NULL;

static int condition_flag = 0;
static wait_queue_head_t wait_queue;
static char message[] = "Condition met! You are now awake.\n";

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "waitq_char: read() called, checking condition...\n");

    // Sleep if condition_flag is not set
    wait_event_interruptible(wait_queue, condition_flag != 0);

    condition_flag = 0; // Reset flag after waking up
    if (copy_to_user(buffer, message, sizeof(message))) {
        return -EFAULT;
    }

    return sizeof(message);
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    // Simulate setting the condition (e.g., from user input)
    condition_flag = 1;
    wake_up_interruptible(&wait_queue);
    printk(KERN_INFO "waitq_char: write() called, condition set, waking up readers.\n");
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write,
};

static int __init waitq_module_init(void) {
    printk(KERN_INFO "waitq_char: Initializing...\n");

    // Allocate major number
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register char device\n");
        return major;
    }

    // Create device class and device
    waitq_class = class_create(CLASS_NAME);
    if (IS_ERR(waitq_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(waitq_class);
    }

    waitq_device = device_create(waitq_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(waitq_device)) {
        class_destroy(waitq_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(waitq_device);
    }

    // Initialize wait queue
    init_waitqueue_head(&wait_queue);

    printk(KERN_INFO "waitq_char: Module loaded. Major: %d\n", major);
    return 0;
}

static void __exit waitq_module_exit(void) {
    device_destroy(waitq_class, MKDEV(major, 0));
    class_unregister(waitq_class);
    class_destroy(waitq_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "waitq_char: Module unloaded.\n");
}

module_init(waitq_module_init);
module_exit(waitq_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_DESCRIPTION("Character Device with Wait Queue Example");

