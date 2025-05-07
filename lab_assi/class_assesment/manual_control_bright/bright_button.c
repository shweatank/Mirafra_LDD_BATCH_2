#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#define DEVICE_NAME "brightness_control"
#define BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/brightness"
#define MAX_BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/max_brightness"

// Structure to hold control state
struct brightness_control {
    int brightness;         // Current brightness level
    int max_brightness;     // Maximum brightness level
    bool ctrl_pressed;      // Track Ctrl key state
};

// Global state
static struct brightness_control control_state;
static struct kobject *brightness_control_kobj;

// Read sysfs file (for integers)
static int read_sysfs_int(const char *path, int *value) {
    struct file *filp;
    char buf[16];
    ssize_t len;
    loff_t pos = 0;

    filp = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(filp))
        return PTR_ERR(filp);

    len = kernel_read(filp, buf, sizeof(buf) - 1, &pos);
    filp_close(filp, NULL);
    if (len <= 0)
        return -EIO;

    buf[len] = '\0';
    return kstrtoint(buf, 10, value);
}

// Write sysfs file (for integers)
static int write_sysfs_int(const char *path, int value) {
    struct file *filp;
    char buf[16];
    ssize_t len;
    loff_t pos = 0;

    filp = filp_open(path, O_WRONLY, 0);
    if (IS_ERR(filp))
        return PTR_ERR(filp);

    snprintf(buf, sizeof(buf), "%d\n", value);
    len = kernel_write(filp, buf, strlen(buf), &pos);
    filp_close(filp, NULL);
    return len > 0 ? 0 : -EIO;
}

// Update the brightness based on the current value
static void update_brightness(void) {
    int new_brightness;

    if (control_state.ctrl_pressed) {
        // Increase brightness
        if (control_state.brightness < control_state.max_brightness) {
            new_brightness = control_state.brightness + control_state.max_brightness / 10;
            if (new_brightness > control_state.max_brightness)
                new_brightness = control_state.max_brightness;
            control_state.brightness = new_brightness;
            write_sysfs_int(BRIGHTNESS_PATH, control_state.brightness);
        }
    } else {
        // Decrease brightness
        if (control_state.brightness > 0) {
            new_brightness = control_state.brightness - control_state.max_brightness / 10;
            if (new_brightness < 0)
                new_brightness = 0;
            control_state.brightness = new_brightness;
            write_sysfs_int(BRIGHTNESS_PATH, control_state.brightness);
        }
    }
}

// Keyboard event handler
static void kbd_handler(int keycode, int value, void *data) {
    // Track Ctrl key state
    if (keycode == KEY_LEFTCTRL) {
        control_state.ctrl_pressed = !!value;
        printk(KERN_INFO "Ctrl key state changed: %d\n", control_state.ctrl_pressed);
        return;
    }

    // Handle key press when Ctrl is pressed
    if (control_state.ctrl_pressed) {
        if (keycode == KEY_PAGEUP && value) { // Ctrl + Page Up (increase brightness)
            printk(KERN_INFO "Ctrl + Page Up pressed\n");
            if (control_state.brightness < control_state.max_brightness) {
                control_state.brightness += control_state.max_brightness / 10;
                if (control_state.brightness > control_state.max_brightness)
                    control_state.brightness = control_state.max_brightness;
                write_sysfs_int(BRIGHTNESS_PATH, control_state.brightness);
            }
        } else if (keycode == KEY_PAGEDOWN && value) { // Ctrl + Page Down (decrease brightness)
            if (control_state.brightness > 0) {
                control_state.brightness -= control_state.max_brightness / 10;
                if (control_state.brightness < 0)
                    control_state.brightness = 0;
                write_sysfs_int(BRIGHTNESS_PATH, control_state.brightness);
            }
        }
    }
}

// Input event handler
static void input_event_handler(struct input_handle *handle, unsigned int type,
                                unsigned int code, int value) {
    if (type == EV_KEY)
        kbd_handler(code, value, NULL);
}

// Input connect/disconnect handlers
static int input_connect(struct input_handler *handler, struct input_dev *dev,
                         const struct input_device_id *id) {
    struct input_handle *handle;
    int error;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = DEVICE_NAME;

    error = input_register_handle(handle);
    if (error)
        goto err_free_handle;

    error = input_open_device(handle);
    if (error)
        goto err_unregister_handle;

    printk(KERN_INFO "Connected to input device\n");
    return 0;

err_unregister_handle:
    input_unregister_handle(handle);
err_free_handle:
    kfree(handle);
    return error;
}

static void input_disconnect(struct input_handle *handle) {
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
    printk(KERN_INFO "Disconnected from input device\n");
}

// Input device ID table
static const struct input_device_id input_ids[] = {
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) },
    },
    { }, // Terminating entry
};

// Input handler structure
static struct input_handler input_handler = {
    .event = input_event_handler,
    .connect = input_connect,
    .disconnect = input_disconnect,
    .name = DEVICE_NAME,
    .id_table = input_ids,
};

// Misc device for user-space interaction
static struct miscdevice misc_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = NULL, // No need for user-space read/write interactions
};

static int __init brightness_control_init(void) {
    int error;

    printk(KERN_INFO "Initializing Brightness Control module...\n");

    // Initialize brightness
    if (read_sysfs_int(MAX_BRIGHTNESS_PATH, &control_state.max_brightness) < 0 ||
        read_sysfs_int(BRIGHTNESS_PATH, &control_state.brightness) < 0) {
        printk(KERN_ERR "Failed to read brightness settings\n");
        return -ENODEV;
    }

    printk(KERN_INFO "Brightness Control loaded\n");
    printk(KERN_INFO "Max brightness: %d\n", control_state.max_brightness);
    printk(KERN_INFO "Current brightness: %d\n", control_state.brightness);

    // Register input handler
    error = input_register_handler(&input_handler);
    if (error) {
        printk(KERN_ERR "Failed to register input handler: %d\n", error);
        return error;
    }

    // Register misc device
    error = misc_register(&misc_dev);
    if (error) {
        printk(KERN_ERR "Failed to register misc device: %d\n", error);
        input_unregister_handler(&input_handler);
        return error;
    }

    printk(KERN_INFO "Brightness Control module successfully loaded.\n");
    return 0;
}

static void __exit brightness_control_exit(void) {
    misc_deregister(&misc_dev);
    input_unregister_handler(&input_handler);
    printk(KERN_INFO "Brightness Control module unloaded\n");
}

module_init(brightness_control_init);
module_exit(brightness_control_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("team1");
MODULE_DESCRIPTION("Kernel module to control brightness with Ctrl + Page Up/Down");

