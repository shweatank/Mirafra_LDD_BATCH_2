#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define BRIGHTNESS_STEP 5 // Percentage step for brightness adjustment

static char backlight_path[256];
static int max_brightness = -1;

static void print_brightness_bar(int brightness, int max) {
    char bar[32] = "[";
    int percentage = (brightness * 100) / max;
    int bars = percentage / 5; // Each bar represents ~5%
    int i;

    for (i = 1; i <= 20; i++) {
        bar[i] = (i <= bars) ? '#' : ' ';
    }
    bar[21] = ']';
    bar[22] = '\0';
    printk(KERN_INFO "brightctl: Brightness %d/%d (%d%%) %s\n", brightness, max, percentage, bar);
}

static int read_sysfs_int(const char *path) {
    struct file *file;
    char buf[16];
    int value = -1;
    loff_t pos = 0;
    ssize_t ret;

    file = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "brightctl: Failed to open %s: %ld\n", path, PTR_ERR(file));
        return -1;
    }

    ret = kernel_read(file, buf, sizeof(buf) - 1, &pos);
    if (ret > 0) {
        buf[ret] = '\0';
        if (kstrtoint(buf, 10, &value) < 0) {
            printk(KERN_ERR "brightctl: Failed to parse %s\n", path);
            value = -1;
        }
    }

    filp_close(file, NULL);
    return value;
}

static int write_sysfs_int(const char *path, int value) {
    struct file *file;
    char buf[16];
    loff_t pos = 0;
    ssize_t ret;

    file = filp_open(path, O_WRONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "brightctl: Failed to open %s for writing: %ld\n", path, PTR_ERR(file));
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d", value);
    ret = kernel_write(file, buf, strlen(buf), &pos);
    if (ret < 0) {
        printk(KERN_ERR "brightctl: Failed to write %d to %s: %ld\n", value, path, ret);
    }

    filp_close(file, NULL);
    return ret >= 0 ? 0 : -1;
}

static int find_backlight_device(void) {
    snprintf(backlight_path, sizeof(backlight_path), "/sys/class/backlight/intel_backlight");

    char max_brightness_path[256];
    snprintf(max_brightness_path, sizeof(max_brightness_path), "%s/max_brightness", backlight_path);

    max_brightness = read_sysfs_int(max_brightness_path);
    if (max_brightness < 0) {
        printk(KERN_ERR "brightctl: intel_backlight not found or unreadable\n");
        return -1;
    }

    printk(KERN_INFO "brightctl: Using intel_backlight at %s\n", backlight_path);
    return 0;
}

static void adjust_brightness(int increment) {
    char brightness_path[256];
    int current_brightness;

    if (max_brightness < 0) {
        printk(KERN_ERR "brightctl: Max brightness not initialized\n");
        return;
    }

    snprintf(brightness_path, sizeof(brightness_path), "%s/brightness", backlight_path);
    current_brightness = read_sysfs_int(brightness_path);
    if (current_brightness < 0) {
        return;
    }

    int step = (max_brightness * BRIGHTNESS_STEP) / 100;
    if (step < 1) step = 1;

    current_brightness += increment * step;
    if (current_brightness < 0) current_brightness = 0;
    if (current_brightness > max_brightness) current_brightness = max_brightness;

    if (write_sysfs_int(brightness_path, current_brightness) == 0) {
        print_brightness_bar(current_brightness, max_brightness);
    }
}

static void brightctl_input_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    static int alt_pressed = 0;

    if (type == EV_KEY) {
        if (code == KEY_LEFTALT || code == KEY_RIGHTALT) {
            alt_pressed = value;
            printk(KERN_INFO "brightctl: Alt key %s\n", value ? "pressed" : "released");
        } else if (alt_pressed && value == 1) {
            if (code == KEY_UP) {
                printk(KERN_INFO "brightctl: Alt+Up detected\n");
                adjust_brightness(1);
            } else if (code == KEY_DOWN) {
                printk(KERN_INFO "brightctl: Alt+Down detected\n");
                adjust_brightness(-1);
            }
        }
    }
}

static int brightctl_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id) {
    struct input_handle *handle;
    int error;

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "brightctl";

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

    printk(KERN_INFO "brightctl: Connected to input device %s\n", dev_name(&dev->dev));
    return 0;
}

static void brightctl_disconnect(struct input_handle *handle) {
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id brightctl_ids[] = {
    { .driver_info = 1 }, // Match all input devices
    { },
};

static struct input_handler brightctl_handler = {
    .event = brightctl_input_event,
    .connect = brightctl_connect,
    .disconnect = brightctl_disconnect,
    .name = "brightctl",
    .id_table = brightctl_ids,
};

static int __init brightctl_init(void) {
    int error;

    error = find_backlight_device();
    if (error) {
        printk(KERN_ERR "brightctl: Failed to initialize backlight device\n");
        return error;
    }

    error = input_register_handler(&brightctl_handler);
    if (error) {
        printk(KERN_ERR "brightctl: Failed to register input handler\n");
        return error;
    }

    printk(KERN_INFO "brightctl: Module loaded\n");
    return 0;
}

static void __exit brightctl_exit(void) {
    input_unregister_handler(&brightctl_handler);
    printk(KERN_INFO "brightctl: Module unloaded\n");
}

module_init(brightctl_init);
module_exit(brightctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash");
MODULE_DESCRIPTION("Kernel-space backlight brightness control (Intel only)");

