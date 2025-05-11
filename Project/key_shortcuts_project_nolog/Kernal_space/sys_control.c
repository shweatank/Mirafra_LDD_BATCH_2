#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/vfs.h>
#include <linux/file.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define DEVICE_NAME "sys_control"
#define MAX_BUFFER 256
#define BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/brightness"
#define MAX_BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/max_brightness"
#define CPU_GOVERNOR_PATH "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"

// Structure to hold control state
struct sys_control {
    char buffer[MAX_BUFFER]; // Buffer for ASCII feedback
    int brightness;         // Current brightness level
    int max_brightness;     // Maximum brightness level
    int volume;             // Current volume level (0-100)
    bool alt_pressed;       // Track Alt key state
    bool ctrl_pressed;      // Track Ctrl key state
    int mode;               // 0: brightness, 1: volume, 2: CPU governor
};

// Global variables
static struct sys_control control_state;
static struct kobject *sys_control_kobj;
static char volume_cmd[8] = "";

// Sysfs attribute for volume command
static ssize_t volume_cmd_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%s\n", volume_cmd);
}

static ssize_t volume_cmd_store(struct kobject *kobj, struct kobj_attribute *attr,
                               const char *buf, size_t count) {
    strncpy(volume_cmd, buf, sizeof(volume_cmd) - 1);
    volume_cmd[sizeof(volume_cmd) - 1] = '\0';
    printk(KERN_INFO "Volume command set: %s\n", volume_cmd);
    return count;
}

static struct kobj_attribute volume_cmd_attr =
    __ATTR(volume_cmd, 0664, volume_cmd_show, volume_cmd_store);

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

// Write sysfs file (for strings)
static int write_sysfs_str(const char *path, const char *value) {
    struct file *filp;
    ssize_t len;
    loff_t pos = 0;

    filp = filp_open(path, O_WRONLY, 0);
    if (IS_ERR(filp))
        return PTR_ERR(filp);

    len = kernel_write(filp, value, strlen(value), &pos);
    filp_close(filp, NULL);
    return len > 0 ? 0 : -EIO;
}

// Update ASCII feedback
static void update_feedback(void) {
    char *buf = control_state.buffer;
    int percentage, bars, i;

    switch (control_state.mode) {
    case 0: // Brightness
        percentage = (control_state.brightness * 100) / control_state.max_brightness;
        bars = percentage / 10;
        snprintf(buf, MAX_BUFFER, "Brightness: [");
        buf += strlen("Brightness: [");
        for (i = 0; i < 10; i++)
            buf[i] = i < bars ? '#' : ' ';
        buf += 10;
        snprintf(buf, MAX_BUFFER - strlen("Brightness: [] "), "] %d%%\n", percentage);
        break;
    case 1: // Volume
        percentage = control_state.volume;
        bars = percentage / 10;
        snprintf(buf, MAX_BUFFER, "Volume: [");
        buf += strlen("Volume: [");
        for (i = 0; i < 10; i++)
            buf[i] = i < bars ? '#' : ' ';
        buf += 10;
        snprintf(buf, MAX_BUFFER - strlen("Volume: [] "), "] %d%%\n", percentage);
        break;
    case 2: // CPU Governor
        snprintf(buf, MAX_BUFFER, "CPU Governor: %s\n",
                 control_state.brightness > control_state.max_brightness / 2 ?
                 "performance" : "powersave");
        break;
    }
}

// File operations for user-space interaction
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *ppos) {
    if (len > MAX_BUFFER)
        len = MAX_BUFFER;
    if (copy_to_user(buf, control_state.buffer, len))
        return -EFAULT;
    return len;
}

static int dev_open(struct inode *inode, struct file *file) {
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .open = dev_open,
    .release = dev_release,
};

// Write volume command to sysfs
static void write_volume_cmd(const char *cmd) {
    strncpy(volume_cmd, cmd, sizeof(volume_cmd) - 1);
    volume_cmd[sizeof(volume_cmd) - 1] = '\0';
    kobject_uevent(sys_control_kobj, KOBJ_CHANGE); // Notify user-space
    printk(KERN_INFO "Wrote volume command: %s\n", volume_cmd);
}

// Interrupt handler for keyboard events
static void kbd_handler(int keycode, int value, void *data) {
    // Track modifier keys
    if (keycode == KEY_LEFTALT) {
        control_state.alt_pressed = !!value;
        return;
    }
    if (keycode == KEY_LEFTCTRL) {
        control_state.ctrl_pressed = !!value;
        return;
    }

    // Handle key press with modifiers
    if (!value)
        return;

    if (control_state.alt_pressed) {
        switch (keycode) {
        case KEY_UP:
            if (control_state.mode == 0) { // Brightness
                if (control_state.brightness < control_state.max_brightness) {
                    control_state.brightness += control_state.max_brightness / 20;
                    if (control_state.brightness > control_state.max_brightness)
                        control_state.brightness = control_state.max_brightness;
                    write_sysfs_int(BRIGHTNESS_PATH, control_state.brightness);
                }
            } else if (control_state.mode == 1) { // Volume
                if (control_state.volume < 100) {
                    control_state.volume += 5;
                    if (control_state.volume > 100)
                        control_state.volume = 100;
                    write_volume_cmd("up");
                }
            }
            break;
        case KEY_DOWN:
            if (control_state.mode == 0) { // Brightness
                if (control_state.brightness > 0) {
                    control_state.brightness -= control_state.max_brightness / 20;
                    if (control_state.brightness < 0)
                        control_state.brightness = 0;
                    write_sysfs_int(BRIGHTNESS_PATH, control_state.brightness);
                }
            } else if (control_state.mode == 1) { // Volume
                if (control_state.volume > 0) {
                    control_state.volume -= 5;
                    if (control_state.volume < 0)
                        control_state.volume = 0;
                    write_volume_cmd("down");
                }
            }
            break;
        }
    } else if (control_state.ctrl_pressed && keycode == KEY_M) {
        // Switch modes
        control_state.mode = (control_state.mode + 1) % 3;
        if (control_state.mode == 2) {
            // Toggle CPU governor
            write_sysfs_str(CPU_GOVERNOR_PATH,
                            control_state.brightness > control_state.max_brightness / 2 ?
                            "performance" : "powersave");
        }
    }

    // Update feedback
    update_feedback();
    printk(KERN_INFO "Mode: %d, Brightness: %d, Volume: %d\n",
           control_state.mode, control_state.brightness, control_state.volume);
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
    .fops = &fops,
};

static int __init sys_control_init(void) {
    int error;

    // Initialize brightness
    if (read_sysfs_int(MAX_BRIGHTNESS_PATH, &control_state.max_brightness) < 0 ||
        read_sysfs_int(BRIGHTNESS_PATH, &control_state.brightness) < 0) {
        printk(KERN_ERR "Failed to read brightness settings\n");
        return -ENODEV;
    }

    // Initialize volume
    control_state.volume = 50; // Default volume (50%)

    // Create sysfs file
    sys_control_kobj = kobject_create_and_add("sys_control", kernel_kobj);
    if (!sys_control_kobj) {
        printk(KERN_ERR "Failed to create sysfs kobject\n");
        return -ENOMEM;
    }

    error = sysfs_create_file(sys_control_kobj, &volume_cmd_attr.attr);
    if (error) {
        printk(KERN_ERR "Failed to create sysfs file: %d\n", error);
        kobject_put(sys_control_kobj);
        return error;
    }

    control_state.alt_pressed = false;
    control_state.ctrl_pressed = false;
    control_state.mode = 0; // Start in brightness mode
    update_feedback();

    // Register input handler
    error = input_register_handler(&input_handler);
    if (error) {
        printk(KERN_ERR "Failed to register input handler: %d\n", error);
        sysfs_remove_file(sys_control_kobj, &volume_cmd_attr.attr);
        kobject_put(sys_control_kobj);
        return error;
    }

    // Register misc device
    error = misc_register(&misc_dev);
    if (error) {
        printk(KERN_ERR "Failed to register misc device: %d\n", error);
        input_unregister_handler(&input_handler);
        sysfs_remove_file(sys_control_kobj, &volume_cmd_attr.attr);
        kobject_put(sys_control_kobj);
        return error;
    }

    printk(KERN_INFO "System Control loaded\n");
    return 0;
}

static void __exit sys_control_exit(void) {
    misc_deregister(&misc_dev);
    input_unregister_handler(&input_handler);
    if (sys_control_kobj) {
        sysfs_remove_file(sys_control_kobj, &volume_cmd_attr.attr);
        kobject_put(sys_control_kobj);
    }
    printk(KERN_INFO "System Control unloaded\n");
}

module_init(sys_control_init);
module_exit(sys_control_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("team1");
MODULE_DESCRIPTION("Keyboard-driven system control with ASCII feedback");
