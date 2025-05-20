#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>      // copy_to_user
#include <linux/wait.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Input handler sending 'V' key press via /dev/kbd_notify");

#define DEVICE_NAME "kbd_notify"

static struct cdev my_cdev;
static struct class *kbd_class;
static dev_t dev_num;

static char notify = 0;
static int flag = 0;
static DECLARE_WAIT_QUEUE_HEAD(wait_queue);

static bool is_v_key_event(struct input_event *event) {
    return event->type == EV_KEY && event->code == KEY_V;
}

static void my_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    if (type == EV_KEY && code == KEY_V && value == 1) {
        notify = 'V';
        flag = 1;
        wake_up_interruptible(&wait_queue);
        printk(KERN_INFO "V key pressed\n");
    }
}

static int my_connect(struct input_handler *handler, struct input_dev *dev,
                      const struct input_device_id *id) {
    struct input_handle *handle;
    int error;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "v_key_handle";

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

    return 0;
}

static void my_disconnect(struct input_handle *handle) {
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id id_table[] = {
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT | INPUT_DEVICE_ID_MATCH_KEYBIT,
        .evbit = { BIT_MASK(EV_KEY) },
        .keybit = { [BIT_WORD(KEY_V)] = BIT_MASK(KEY_V) },
    },
    { }, // Terminating entry
};

static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    // Wait until flag is set by event
    if (wait_event_interruptible(wait_queue, flag != 0))
        return -ERESTARTSYS;

    flag = 0;

    if (copy_to_user(buf, &notify, 1))
        return -EFAULT;

    return 1;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
};

static struct input_handler my_input_handler = {
    .event      = my_event,
    .connect    = my_connect,
    .disconnect = my_disconnect,
    .name       = "v_key_handler",
    .id_table   = id_table,
};

static int __init my_init(void) {
    int ret;

    ret = input_register_handler(&my_input_handler);
    if (ret) {
        printk(KERN_ERR "Failed to register input handler\n");
        return ret;
    }

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        input_unregister_handler(&my_input_handler);
        return ret;
    }

    cdev_init(&my_cdev, &fops);

    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        input_unregister_handler(&my_input_handler);
        return ret;
    }

    kbd_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(kbd_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_num, 1);
        input_unregister_handler(&my_input_handler);
        return PTR_ERR(kbd_class);
    }

    device_create(kbd_class, NULL, dev_num, NULL, DEVICE_NAME);

    printk(KERN_INFO "kbd_notify module loaded, device /dev/%s created\n", DEVICE_NAME);

    return 0;
}

static void __exit my_exit(void) {
    device_destroy(kbd_class, dev_num);
    class_destroy(kbd_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    input_unregister_handler(&my_input_handler);
    printk(KERN_INFO "kbd_notify module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

