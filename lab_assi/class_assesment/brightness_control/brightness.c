// File: kb_brightness.c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/backlight.h>
#include <linux/notifier.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_DESCRIPTION("Kernel module to control brightness from keyboard");
MODULE_VERSION("1.0");

#define KEY_BRIGHTNESSUP   225
#define KEY_BRIGHTNESSDOWN 224

static struct backlight_device *bl_dev = NULL;
static struct notifier_block nb;

static int adjust_brightness(int direction)
{
    int new_brightness;

    if (!bl_dev)
        return -ENODEV;

    new_brightness = bl_dev->props.brightness + direction;

    if (new_brightness < 0)
        new_brightness = 0;
    else if (new_brightness > bl_dev->props.max_brightness)
        new_brightness = bl_dev->props.max_brightness;

    bl_dev->props.brightness = new_brightness;
    return backlight_update_status(bl_dev);
}

static int input_event_notify(struct notifier_block *nb, unsigned long type, void *data)
{
    struct input_value *iv;

    struct input_handle *handle = data;

    if (type != EV_KEY)
        return NOTIFY_OK;

    for (iv = handle->dev->vals; iv < handle->dev->vals + handle->dev->num_vals; iv++) {
        if (iv->type == EV_KEY && iv->value == 1) { // key press only
            if (iv->code == KEY_BRIGHTNESSUP) {
                adjust_brightness(10);
            } else if (iv->code == KEY_BRIGHTNESSDOWN) {
                adjust_brightness(-10);
            }
        }
    }

    return NOTIFY_OK;
}

static int __init kb_brightness_init(void)
{
    pr_info("kb_brightness: initializing module...\n");

    bl_dev = backlight_device_get_by_name("intel_backlight");
    if (!bl_dev) {
        pr_err("kb_brightness: backlight device not found\n");
        return -ENODEV;
    }

    nb.notifier_call = input_event_notify;
    input_register_handler(&nb);

    pr_info("kb_brightness: module loaded\n");
    return 0;
}

static void __exit kb_brightness_exit(void)
{
    input_unregister_handler(&nb);
    pr_info("kb_brightness: module unloaded\n");
}

module_init(kb_brightness_init);
module_exit(kb_brightness_exit);
