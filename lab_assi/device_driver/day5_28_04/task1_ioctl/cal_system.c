#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/keyboard.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#define DEVICE_NAME "calculation"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)
#define KEYBOARD_IRQ 1

struct calc_data {
    int num1;
    int num2;
    int result;
};

// Global variables
static int major;
static struct calc_data *calc_buffer;
static char dev_name[] = "keyboard_interrupt_dev"; // Correct dev_id for IRQF_SHARED

// Keyboard interrupt handler
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    if (!calc_buffer)
        return IRQ_HANDLED; // Ensure calc_buffer is valid

    scancode = inb(0x60); // Read scancode from keyboard controller
    if (scancode & 0x80)
        return IRQ_HANDLED; // Ignore key release

    pr_info("Keyboard interrupt received! Scancode: 0x%02x\n", scancode);


    switch (scancode) {
        case 0x1e: // 'a' key press
            calc_buffer->result = calc_buffer->num1 + calc_buffer->num2;
            pr_info("Operation: %d + %d = %d\n", calc_buffer->num1, calc_buffer->num2, calc_buffer->result);
            break;
        case 0x30: // 'b' key press
            calc_buffer->result = calc_buffer->num1 - calc_buffer->num2;
            pr_info("Operation: %d - %d = %d\n", calc_buffer->num1, calc_buffer->num2, calc_buffer->result);
            break;
        case 0x2e: // 'c' key press
            calc_buffer->result = calc_buffer->num1 * calc_buffer->num2;
            pr_info("Operation: %d * %d = %d\n", calc_buffer->num1, calc_buffer->num2, calc_buffer->result);
            break;
        default:
            calc_buffer->result = 0;
            pr_info("Invalid key pressed. Result set to 0.\n");
            break;
    }

    return IRQ_HANDLED;
}

// IOCTL function
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    if (!calc_buffer)
        return -EFAULT;

    switch (cmd) {
        case IOCTL_SET_DATA:
            if (copy_from_user(calc_buffer, (struct calc_data __user *)arg, sizeof(struct calc_data)))
                ret = -EFAULT;
            else
                pr_info("Received numbers: %d and %d\n", calc_buffer->num1, calc_buffer->num2);
            break;

        case IOCTL_GET_RESULT:
            if (copy_to_user((int __user *)arg, &calc_buffer->result, sizeof(int)))
                ret = -EFAULT;
            break;

        default:
            ret = -EINVAL;
            break;
    }
    return ret;
}

// Device open
static int dev_open(struct inode *inodep, struct file *filep)
{
    calc_buffer = kmalloc(sizeof(struct calc_data), GFP_KERNEL);
    if (!calc_buffer) {
        pr_err("Failed to allocate memory\n");
        return -ENOMEM;
    }

    pr_info("Device opened\n");
    return 0;
}

// Device release
static int dev_release(struct inode *inodep, struct file *filep)
{
    if (calc_buffer) {
        kfree(calc_buffer);
        calc_buffer = NULL;
        pr_info("Memory freed\n");
    }

    pr_info("Device closed\n");
    return 0;
}

// File operations
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

// Module initialization
static int __init simple_calculator_init(void)
{
    int ret;

    pr_info("Loading Simple Calculator Module\n");

    // Register IRQ handler
    ret = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, "keyboard_interrupt", dev_name);
    if (ret) {
        pr_err("Failed to register keyboard IRQ\n");
        return ret;
    }

    // Register character device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register device\n");
        free_irq(KEYBOARD_IRQ, dev_name);
        return major;
    }

    pr_info("Simple Calculator Loaded (Major: %d)\n", major);
    return 0;
}

// Module cleanup
static void __exit simple_calculator_exit(void)
{
    free_irq(KEYBOARD_IRQ, dev_name);
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("Simple Calculator Module Unloaded\n");
}

module_init(simple_calculator_init);
module_exit(simple_calculator_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_VERSION("2.0");

