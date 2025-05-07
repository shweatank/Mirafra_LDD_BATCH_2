#include <linux/module.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/backlight.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

#define KEY_BRIGHTNESSUP   225
#define KEY_BRIGHTNESSDOWN 224

static struct backlight_device *bl_dev = NULL;

// Handle key events
static void handle_key_event(struct input_handle *handle, unsigned int code, int value)
{
    int new_brightness;

    if (value != 1) // only handle key press
        return;

    if (!bl_dev)
        return;

    new_brightness = bl_dev->props.brightness;

    if (code == KEY_BRIGHTNESSUP) {
        new_brightness += 10;
    } else if (code == KEY_BRIGHTNESSDOWN) {
        new_brightness -= 10;
    }

    // Clamp and set brightness
    new_brightness = clamp(new_brightness, 0, bl_dev->props.max_brightness);
    bl_dev->props.brightness = new_brightness;
    backlight_update_status(bl_dev);
}

// Event function for input handler
static bool is_keyboard(struct input_dev *dev)
{
    return test_bit(EV_KEY, dev->evbit) && test_bit(KEY_SPACE, dev->keybit);
}

static int kb_connect(struct input_handler *handler, struct input_dev *dev,
                      const struct input_device_id *id)
{
    struct input_handle *handle;

    if (!is_keyboard(dev))
        return -ENODEV;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "kb_brightness";

    input_register_handle(handle);
    input_open_device(handle);

    return 0;
}

static void kb_disconnect(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static void kb_event(struct input_handle *handle, unsigned int type,
                    unsigned int code, int value)
{
    if (type == EV_KEY)
        handle_key_event(handle, code, value);

}

static const struct input_device_id kb_ids[] = {
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) },
    },
    { }, // terminating entry
};

MODULE_DEVICE_TABLE(input, kb_ids);

static struct input_handler kb_input_handler = {
    .event     = kb_event,
    .connect   = kb_connect,
    .disconnect = kb_disconnect,
    .name      = "kb_brightness_handler",
    .id_table  = kb_ids,
};

static int __init kb_brightness_init(void)
{
    pr_info("kb_brightness: initializing...\n");

    bl_dev = backlight_device_get_by_name("intel_backlight");
    if (!bl_dev) {
        pr_err("Backlight device not found\n");
        return -ENODEV;
    }

    return input_register_handler(&kb_input_handler);
}

static void __exit kb_brightness_exit(void)
{
    input_unregister_handler(&kb_input_handler);
    pr_info("kb_brightness: exiting...\n");
}

module_init(kb_brightness_init);
module_exit(kb_brightness_exit);

