#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/slab.h>

#define DEVICE_NAME "volctl"
#define CLASS_NAME "volctl_class"

static dev_t dev_number;
static struct cdev cdev;
static struct class *volctl_class;
static struct device *volctl_device;
static char message[8] = {0};
static int msg_ready = 0;
static struct input_dev *input_dev;

static int volctl_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "volctl: Device opened\n");
    return 0;
}

static int volctl_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "volctl: Device released\n");
    return 0;
}

static ssize_t volctl_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (!msg_ready)
        return 0;
    if (len < strlen(message))
        return -EINVAL;
    if (copy_to_user(buf, message, strlen(message)))
        return -EFAULT;
    printk(KERN_INFO "volctl: Sent message to user: %s\n", message);
    msg_ready = 0;
    return strlen(message);
}

static struct file_operations fops = {
    .open = volctl_open,
    .release = volctl_release,
    .read = volctl_read,
};

static void volctl_input_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    static int alt_pressed = 0;

    if (type == EV_KEY) {
        if (code == KEY_LEFTALT || code == KEY_RIGHTALT) {
            alt_pressed = value;
            printk(KERN_INFO "volctl: Alt key %s\n", value ? "pressed" : "released");
        } else if (alt_pressed && value == 1) { // Key press
            if (code == KEY_UP) {
                strcpy(message, "UP");
                msg_ready = 1;
                printk(KERN_INFO "volctl: Alt+Up detected\n");
            } else if (code == KEY_DOWN) {
                strcpy(message, "DOWN");
                msg_ready = 1;
                printk(KERN_INFO "volctl: Alt+Down detected\n");
            }
        }
        printk(KERN_INFO "volctl: Key event - code: %u, value: %d, alt: %d\n", code, value, alt_pressed);
    }
}

static int volctl_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id) {
    struct input_handle *handle;
    int error;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "volctl";

    error = input_register_handle(handle);
    if (error) {
        kfree(handle);
        return error;
    }

    error = input_open_device(handle);
    if (error) {
        input_unregister_handle(handle);
        kfree(handle);
        return error;
    }

    printk(KERN_INFO "volctl: Connected to input device %s\n", dev_name(&dev->dev));
    return 0;
}

static void volctl_disconnect(struct input_handle *handle) {
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id volctl_ids[] = {
    { .driver_info = 1 }, // Match all input devices
    { },
};

static struct input_handler volctl_handler = {
    .event = volctl_input_event,
    .connect = volctl_connect,
    .disconnect = volctl_disconnect,
    .name = "volctl",
    .id_table = volctl_ids,
};

static int __init volctl_init(void) {
    int error;

    if (alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ERR "volctl: Failed to allocate major number\n");
        return -1;
    }
    cdev_init(&cdev, &fops);
    if (cdev_add(&cdev, dev_number, 1) < 0) {
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_ERR "volctl: Failed to add cdev\n");
        return -1;
    }
    volctl_class = class_create(CLASS_NAME);
    if (IS_ERR(volctl_class)) {
        cdev_del(&cdev);
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_ERR "volctl: Failed to create class\n");
        return PTR_ERR(volctl_class);
    }
    volctl_device = device_create(volctl_class, NULL, dev_number, NULL, DEVICE_NAME);
    if (IS_ERR(volctl_device)) {
        class_destroy(volctl_class);
        cdev_del(&cdev);
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_ERR "volctl: Failed to create device\n");
        return PTR_ERR(volctl_device);
    }

    error = input_register_handler(&volctl_handler);
    if (error) {
        device_destroy(volctl_class, dev_number);
        class_destroy(volctl_class);
        cdev_del(&cdev);
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_ERR "volctl: Failed to register input handler\n");
        return error;
    }

    printk(KERN_INFO "volctl: Module loaded\n");
    return 0;
}

static void __exit volctl_exit(void) {
    input_unregister_handler(&volctl_handler);
    device_destroy(volctl_class, dev_number);
    class_destroy(volctl_class);
    cdev_del(&cdev);
    unregister_chrdev_region(dev_number, 1);
    printk(KERN_INFO "volctl: Module unloaded\n");
}

module_init(volctl_init);
module_exit(volctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grok");
MODULE_DESCRIPTION("ALSA volume control via input subsystem");
