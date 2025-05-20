#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#define DEVICE_NAME "panic_button"
#define MAGIC_NUM 100
#define IOCTL_GET_PANIC_FLAG _IOR(MAGIC_NUM, 1, int *)
#define KEYBOARD_IRQ 1

static int major;
static int panic_flag = 0;
static int esc_press_count = 0;
static char dev_name[] = "keyboard_interrupt_panic_dev";

// Keyboard interrupt handler
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(0x60);

    // Only process key press (not release)
    if (scancode & 0x80)
        return IRQ_HANDLED;

    if (scancode == 0x01) {  // 'Esc' key press
        esc_press_count++;
        pr_info("ESC key press count: %d\n", esc_press_count);
        if (esc_press_count >= 3) {
            panic_flag = 1;
            pr_alert("Panic Button Triggered! (Triple ESC)\n");
            esc_press_count = 0;  // reset count after detection
        }
    } else {
        // Reset counter if different key is pressed
        if (esc_press_count > 0)
            pr_info("Resetting ESC press count due to another key\n");
        esc_press_count = 0;
    }

    return IRQ_HANDLED;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case IOCTL_GET_PANIC_FLAG:
            if (copy_to_user((int __user *)arg, &panic_flag, sizeof(int)))
                return -EFAULT;
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("Panic Button device opened\n");
    panic_flag = 0;
    esc_press_count = 0;
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("Panic Button device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

static int __init panic_button_init(void)
{
    int ret;
    pr_info("Loading Panic Button Module...\n");

    ret = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, "keyboard_interrupt", dev_name);
    if (ret) {
        pr_err("Failed to register keyboard IRQ\n");
        return ret;
    }

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        free_irq(KEYBOARD_IRQ, dev_name);
        return major;
    }

    pr_info("Panic Button Module Loaded (Major: %d)\n", major);
    return 0;
}

static void __exit panic_button_exit(void)
{
    free_irq(KEYBOARD_IRQ, dev_name);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Panic Button Module Unloaded\n");
}

module_init(panic_button_init);
module_exit(panic_button_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Interrupt");
MODULE_DESCRIPTION("Detects triple 'Esc' key presses as Panic Button");

