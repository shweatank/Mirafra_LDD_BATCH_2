#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "audio_trigger"
static int major;

static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "audio_trigger: Trigger received from user space\n");
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = dev_write,
};

static int __init audio_trigger_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "audio_trigger: Failed to register device\n");
        return major;
    }
    printk(KERN_INFO "audio_trigger: Registered with major number %d\n", major);
    return 0;
}

static void __exit audio_trigger_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "audio_trigger: Unregistered\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kavya KL");
MODULE_DESCRIPTION("Kernel module to trigger audio");
module_init(audio_trigger_init);
module_exit(audio_trigger_exit);

