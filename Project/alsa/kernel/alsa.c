#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "audio_trigger"
#define CLASS_NAME "audio_trigger_class"

static int major_number;
static struct class *audio_trigger_class = NULL;
static struct device *audio_trigger_device = NULL;
static struct cdev audio_trigger_cdev;
static char trigger = 0; // Trigger state (0 or 1)

static int audio_trigger_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "audio_trigger: Device opened\n");
    return 0;
}

static int audio_trigger_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "audio_trigger: Device closed\n");
    return 0;
}

static ssize_t audio_trigger_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (*offset > 0) return 0; // Already read
    if (len < 1) return -EINVAL;
    if (copy_to_user(buf, &trigger, 1)) return -EFAULT;
    *offset += 1;
    trigger = 0; // Reset trigger after read
    return 1;
}

static ssize_t audio_trigger_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    char val;
    if (len < 1) return -EINVAL;
    if (copy_from_user(&val, buf, 1)) return -EFAULT;
    trigger = (val == '1') ? 1 : 0; // Set trigger if "1" is written
    printk(KERN_INFO "audio_trigger: Trigger set to %d\n", trigger);
    return len;
}

static struct file_operations fops = {
    .open = audio_trigger_open,
    .release = audio_trigger_release,
    .read = audio_trigger_read,
    .write = audio_trigger_write,
};

static int __init audio_trigger_init(void) {
    // Register character device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "audio_trigger: Failed to register major number\n");
        return major_number;
    }

    // Create device class
    audio_trigger_class = class_create(CLASS_NAME);
    if (IS_ERR(audio_trigger_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "audio_trigger: Failed to create class\n");
        return PTR_ERR(audio_trigger_class);
    }

    // Create device
    audio_trigger_device = device_create(audio_trigger_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(audio_trigger_device)) {
        class_destroy(audio_trigger_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "audio_trigger: Failed to create device\n");
        return PTR_ERR(audio_trigger_device);
    }

    // Initialize cdev
    cdev_init(&audio_trigger_cdev, &fops);
    if (cdev_add(&audio_trigger_cdev, MKDEV(major_number, 0), 1) < 0) {
        device_destroy(audio_trigger_class, MKDEV(major_number, 0));
        class_destroy(audio_trigger_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "audio_trigger: Failed to add cdev\n");
        return -1;
    }

    printk(KERN_INFO "audio_trigger: Module loaded, major number %d\n", major_number);
    return 0;
}

static void __exit audio_trigger_exit(void) {
    cdev_del(&audio_trigger_cdev);
    device_destroy(audio_trigger_class, MKDEV(major_number, 0));
    class_destroy(audio_trigger_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "audio_trigger: Module unloaded\n");
}

module_init(audio_trigger_init);
module_exit(audio_trigger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grok");
MODULE_DESCRIPTION("Simple kernel module to trigger audio playback");
