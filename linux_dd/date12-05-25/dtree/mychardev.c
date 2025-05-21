#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>

#define DEVICE_NAME "mychardev"
#define CLASS_NAME  "mycharclass"

static int major;
static struct class*  charClass  = NULL;
static struct device* charDevice = NULL;
static struct device_node* dev_node = NULL;

static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("mychardev: Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    const char *msg = "Hello from driver\n";
    size_t msg_len = strlen(msg);
    return copy_to_user(buffer, msg, msg_len) ? -EFAULT : msg_len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    pr_info("mychardev: Received %zu bytes from user\n", len);
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("mychardev: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init mychardev_init(void) {
    pr_info("mychardev: Initializing...\n");

    // Read from device tree
    dev_node = of_find_node_by_name(NULL, "mychardev");
    if (!dev_node) {
        pr_warn("mychardev: DTS node not found\n");
    } else {
        const char *label;
        if (!of_property_read_string(dev_node, "label", &label)) {
            pr_info("mychardev: Label from DTS = %s\n", label);
        }
    }

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    //charClass = class_create(THIS_MODULE, CLASS_NAME);//
    charClass = class_create(CLASS_NAME);
    if (IS_ERR(charClass)) {
        unregister_chrdev(major, DEVICE_NAME);
        pr_err("Failed to register device class\n");
        return PTR_ERR(charClass);
    }

    charDevice = device_create(charClass, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(charDevice)) {
        class_destroy(charClass);
        unregister_chrdev(major, DEVICE_NAME);
        pr_err("Failed to create device\n");
        return PTR_ERR(charDevice);
    }

    pr_info("mychardev: Module loaded\n");
    return 0;
}

static void __exit mychardev_exit(void) {
    device_destroy(charClass, MKDEV(major, 0));
    class_unregister(charClass);
    class_destroy(charClass);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("mychardev: Module unloaded\n");
}

module_init(mychardev_init);
module_exit(mychardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple char device with DTS parsing");
MODULE_VERSION("1.0");
