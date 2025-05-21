#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>

#define DEVICE_NAME "simple_calculator"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant");
MODULE_DESCRIPTION("Kernel Calculator using IOCTL and static memory allocation");
MODULE_VERSION("1.0");

struct calc_data {
    int num1;
    int num2;
    int result;
};

#define KEYBOARD_IRQ 1  // Keyboard IRQ number
int count = 0;

// Static buffer for calculation data
static struct calc_data calc_buffer;

// Interrupt handler function
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Keyboard interrupt received! %d\n", count);
    perform_calculation();
    count++;
    if (count == 4) {
        count = 0;
    }

    return IRQ_HANDLED;
}

// Perform calculation based on the operation
static void perform_calculation(void) {
    switch (count) {
        case 0: calc_buffer.result = calc_buffer.num1 + calc_buffer.num2; break;
        case 1: calc_buffer.result = calc_buffer.num1 - calc_buffer.num2; break;
        case 2: calc_buffer.result = calc_buffer.num1 * calc_buffer.num2; break;
        case 3: 
            if (calc_buffer.num2 != 0)
                calc_buffer.result = calc_buffer.num1 / calc_buffer.num2;
            else
                calc_buffer.result = 0;
            break;
        default: calc_buffer.result = 0;
    }
}

// IOCTL function
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_SET_DATA:
            if (copy_from_user(&calc_buffer, (struct calc_data __user *)arg, sizeof(struct calc_data)))
                return -EFAULT;
            break;

        case IOCTL_GET_RESULT:
            if (copy_to_user((int __user *)arg, &calc_buffer.result, sizeof(int)))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

// Device open
static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("Device opened\n");
    return 0;
}

// Device release
static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("Device closed\n");
    return 0;
}

// File operations
static struct file_operations fops = {
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

// Module init
static int __init simple_calculator_init(void) {
    int result;

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register device\n");
        return major;
    }

    // Request IRQ for the keyboard
    result = request_irq(KEYBOARD_IRQ,            // IRQ number
                         keyboard_interrupt_handler, // Interrupt handler
                         IRQF_SHARED,                // Shared IRQ flag
                         "keyboard_interrupt",       // Name
                         (void *)(keyboard_interrupt_handler)); // Device ID for sharing

    if (result) {
        printk(KERN_ERR "Failed to register keyboard IRQ\n");
        unregister_chrdev(major, DEVICE_NAME);
        return result;
    }

    pr_info("Simple Calculator Loaded (Major: %d)\n", major);
    return 0;
}

// Module exit
static void __exit simple_calculator_exit(void) {
    // Free the IRQ
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Simple Calculator Unloaded\n");
}

module_init(simple_calculator_init);
module_exit(simple_calculator_exit);

