#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/videodev2.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define DEVICE_NAME "simple_cam"
#define CLASS_NAME "cam"

static struct class *cam_class = NULL;
static struct cdev cam_cdev;
static dev_t dev_no;

static int cam_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Camera: Device opened\n");
    return 0;
}

static int cam_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Camera: Device closed\n");
    return 0;
}

static ssize_t cam_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    // This is where you would implement capturing an image
    // For simplicity, we're just returning a message
    const char *msg = "Photo captured!\n";
    size_t msg_len = strlen(msg);
    
    if (*off > 0)
        return 0;
    
    if (copy_to_user(buf, msg, msg_len))
        return -EFAULT;
    
    *off = msg_len;
    return msg_len;
}

static struct file_operations fops = {
    .open = cam_open,
    .release = cam_close,
    .read = cam_read,
};

static int __init cam_init(void)
{
    // Allocate device numbers
    if (alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ALERT "Camera: Failed to allocate device number\n");
        return -1;
    }
    
    // Create device class
    cam_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cam_class)) {
        unregister_chrdev_region(dev_no, 1);
        printk(KERN_ALERT "Camera: Failed to create device class\n");
        return PTR_ERR(cam_class);
    }
    
    // Create device
    if (device_create(cam_class, NULL, dev_no, NULL, DEVICE_NAME) == NULL) {
        class_destroy(cam_class);
        unregister_chrdev_region(dev_no, 1);
        printk(KERN_ALERT "Camera: Failed to create device\n");
        return -1;
    }
    
    // Initialize and add character device
    cdev_init(&cam_cdev, &fops);
    if (cdev_add(&cam_cdev, dev_no, 1) == -1) {
        device_destroy(cam_class, dev_no);
        class_destroy(cam_class);
        unregister_chrdev_region(dev_no, 1);
        printk(KERN_ALERT "Camera: Failed to add character device\n");
        return -1;
    }
    
    printk(KERN_INFO "Camera: Module loaded successfully\n");
    return 0;
}

static void __exit cam_exit(void)
{
    cdev_del(&cam_cdev);
    device_destroy(cam_class, dev_no);
    class_destroy(cam_class);
    unregister_chrdev_region(dev_no, 1);
    printk(KERN_INFO "Camera: Module unloaded successfully\n");
}

module_init(cam_init);
module_exit(cam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple Camera Kernel Module");
MODULE_VERSION("0.1");
