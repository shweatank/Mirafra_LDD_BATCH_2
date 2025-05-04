#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>

#define DEVICE_NAME "key_shortcuts"
#define CONFIG_FILE "/etc/key_shortcuts.conf"
#define IOC_MAGIC 'k'
#define IOC_SET_CONFIG _IOW(IOC_MAGIC, 1, struct shortcut_config)
#define CONFIG_VERSION 1

// Configuration structure
struct shortcut_config {
    unsigned int version;
    unsigned int camera_key;
    unsigned int fileman_key;
};

// Module state
static unsigned int camera_key = 0; // Require configuration
static unsigned int fileman_key = 0; // Require configuration
static bool ctrl_key_pressed = false;
static DEFINE_MUTEX(config_lock);
static atomic_t dev_in_use = ATOMIC_INIT(0);

// Input handler
static struct input_handler key_handler;
static struct input_handle *key_handle;

// Execute a user-space command
static void execute_command(const char *cmd) {
    char *argv[] = {"/bin/sh", "-c", (char *)cmd, NULL};
    char *envp[] = {"DISPLAY=:0", NULL};
    printk(KERN_INFO "[key_shortcuts] Attempting to execute command: %s\n", cmd);
    int ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    if (ret) {
        printk(KERN_WARNING "[key_shortcuts] Failed to execute command: %s, error=%d\n", cmd, ret);
    } else {
        printk(KERN_INFO "[key_shortcuts] Successfully executed command: %s\n", cmd);
    }
}

// Input event handler
static void key_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    printk(KERN_DEBUG "[key_shortcuts] Received event: type=%u, code=%u, value=%d, device=%s\n", 
           type, code, value, handle->dev->name);
    if (type != EV_KEY || value > 1) {
        printk(KERN_DEBUG "[key_shortcuts] Ignoring non-key event or invalid value: type=%u, value=%d\n", type, value);
        return;
    }

    if (code == KEY_LEFTCTRL) {
        ctrl_key_pressed = (value == 1);
        printk(KERN_INFO "[key_shortcuts] Ctrl state: %d (code=%u, value=%d)\n", ctrl_key_pressed, code, value);
        return;
    }

    if (ctrl_key_pressed && value == 1) {
        mutex_lock(&config_lock);
        printk(KERN_INFO "[key_shortcuts] Checking shortcut: code=%u, camera_key=%u, fileman_key=%u\n",
               code, camera_key, fileman_key);
        if (code == camera_key) {
            printk(KERN_INFO "[key_shortcuts] Launching camera (key=%u)\n", code);
            execute_command("/usr/bin/cheese");
        } else if (code == fileman_key) {
            printk(KERN_INFO "[key_shortcuts] Launching file manager (key=%u)\n", code);
            execute_command("/usr/bin/nautilus");
        } else {
            printk(KERN_DEBUG "[key_shortcuts] No shortcut match for code=%u\n", code);
        }
        mutex_unlock(&config_lock);
    } else {
        printk(KERN_DEBUG "[key_shortcuts] Not processing: ctrl_key_pressed=%d, value=%d\n", ctrl_key_pressed, value);
    }
}

// Input handler connection
static int key_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id) {
    key_handle = kzalloc(sizeof(*key_handle), GFP_KERNEL);
    if (!key_handle) {
        printk(KERN_ERR "[key_shortcuts] Failed to allocate input handle\n");
        return -ENOMEM;
    }

    key_handle->dev = dev;
    key_handle->handler = handler;
    key_handle->name = DEVICE_NAME;
    int ret = input_register_handle(key_handle);
    if (ret) {
        printk(KERN_ERR "[key_shortcuts] Failed to register input handle, error=%d\n", ret);
        kfree(key_handle);
        return ret;
    }
    ret = input_open_device(key_handle);
    if (ret) {
        printk(KERN_ERR "[key_shortcuts] Failed to open input device, error=%d\n", ret);
        input_unregister_handle(key_handle);
        kfree(key_handle);
        return ret;
    }
    ret = input_grab_device(key_handle);
    if (ret) {
        printk(KERN_ERR "[key_shortcuts] Failed to grab input device, error=%d\n", ret);
        input_close_device(key_handle);
        input_unregister_handle(key_handle);
        kfree(key_handle);
        return ret;
    }
    printk(KERN_INFO "[key_shortcuts] Input handler registered and grabbed for device: %s\n", dev->name);
    return 0;
}

static void key_disconnect(struct input_handle *handle) {
    printk(KERN_INFO "[key_shortcuts] Input handler disconnected for device: %s\n", handle->dev->name);
    input_release_device(handle);
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

// Input device IDs
static const struct input_device_id key_ids[] = {
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) },
    },
    { },
};

// Character device operations
static int dev_open(struct inode *inode, struct file *file) {
    if (atomic_inc_return(&dev_in_use) > 1) {
        atomic_dec(&dev_in_use);
        return -EBUSY;
    }
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "[key_shortcuts] Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    atomic_dec(&dev_in_use);
    module_put(THIS_MODULE);
    printk(KERN_INFO "[key_shortcuts] Device released\n");
    return 0;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct shortcut_config config;
    if (cmd != IOC_SET_CONFIG) {
        printk(KERN_WARNING "[key_shortcuts] Invalid ioctl command: %u\n", cmd);
        return -EINVAL;
    }
    printk(KERN_INFO "[key_shortcuts] Attempting to copy config from user, size=%zu\n", sizeof(config));
    if (copy_from_user(&config, (void __user *)arg, sizeof(config))) {
        printk(KERN_WARNING "[key_shortcuts] Failed to copy ioctl data\n");
        return -EFAULT;
    }
    printk(KERN_INFO "[key_shortcuts] Received config: version=%u, camera_key=%u, fileman_key=%u\n",
           config.version, config.camera_key, config.fileman_key);
    if (config.version != CONFIG_VERSION) {
        printk(KERN_WARNING "[key_shortcuts] Invalid config version: %u, expected: %u\n",
               config.version, CONFIG_VERSION);
        return -EINVAL;
    }
    if (config.camera_key == 0 || config.fileman_key == 0) {
        printk(KERN_WARNING "[key_shortcuts] Invalid keycodes: camera=%u, fileman=%u\n",
               config.camera_key, config.fileman_key);
        return -EINVAL;
    }

    mutex_lock(&config_lock);
    camera_key = config.camera_key;
    fileman_key = config.fileman_key;
    mutex_unlock(&config_lock);

    printk(KERN_INFO "[key_shortcuts] Updated shortcuts: camera_key=%u, fileman_key=%u\n",
           camera_key, fileman_key);
    return 0;
}

static const struct file_operations dev_fops = {
    .open = dev_open,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl,
    .owner = THIS_MODULE,
};

// Misc device
static struct miscdevice misc_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,
    .mode = 0600,
};

// Trigger user-space settings program
static void trigger_settings_program(void) {
    char *argv[] = {"/usr/bin/configure_shortcuts", "--init", NULL};
    char *envp[] = {"DISPLAY=:0", NULL};
    int ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
    if (ret)
        printk(KERN_WARNING "[key_shortcuts] Failed to trigger settings program, error=%d\n", ret);
    else
        printk(KERN_INFO "[key_shortcuts] Triggered settings program\n");
}

// Check config file existence
static bool config_file_exists(void) {
    struct file *file = filp_open(CONFIG_FILE, O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_INFO "[key_shortcuts] Config file %s not found, using defaults\n", CONFIG_FILE);
        return false;
    }
    filp_close(file, NULL);
    return true;
}

// Input handler structure
static struct input_handler key_handler = {
    .event = key_event,
    .connect = key_connect,
    .disconnect = key_disconnect,
    .name = DEVICE_NAME,
    .id_table = key_ids,
};

static int __init key_shortcuts_init(void) {
    int ret = misc_register(&misc_dev);
    if (ret) {
        printk(KERN_ERR "[key_shortcuts] Failed to register misc device, error=%d\n", ret);
        return ret;
    }
    ret = input_register_handler(&key_handler);
    if (ret) {
        misc_deregister(&misc_dev);
        printk(KERN_ERR "[key_shortcuts] Failed to register input handler, error=%d\n", ret);
        return ret;
    }
    if (!config_file_exists())
        trigger_settings_program();
    printk(KERN_INFO "[key_shortcuts] Key shortcuts module loaded\n");
    return 0;
}

static void __exit key_shortcuts_exit(void) {
    input_unregister_handler(&key_handler);
    misc_deregister(&misc_dev);
    printk(KERN_INFO "[key_shortcuts] Key shortcuts module unloaded\n");
}

module_init(key_shortcuts_init);
module_exit(key_shortcuts_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xAI");
MODULE_DESCRIPTION("Custom keyboard shortcuts kernel module");
