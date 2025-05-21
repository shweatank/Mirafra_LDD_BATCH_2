#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

#define DEVICE_NAME "keyboard_interrupt"
#define MAJOR_NUM 100

static int press_count = 0;  // Counter for keypresses
static float result = 0.0;   // Store the result of operations

// This will store the numbers for operation
static float num1 = 10.0;
static float num2 = 20.0;

static ssize_t keyboard_interrupt_write(struct file *file, const char *buffer, size_t len, loff_t *offset) {
    press_count++;

    switch (press_count % 4) {
        case 1: // Addition
            result = num1 + num2;
            printk(KERN_INFO "Addition: %d + %d = %d\n", (int)num1, (int)num2, (int)result);
            break;
        case 2: // Subtraction
            result = num2 - num1;
            printk(KERN_INFO "Subtraction: %d - %d = %d\n", (int)num2, (int)num1, (int)result);
            break;
        case 3: // Multiplication
            result = num1 * num2;
            printk(KERN_INFO "Multiplication: %d * %d = %d\n", (int)num1, (int)num2, (int)result);
            break;
        case 4: // Division
            if (num1 != 0) {
                result = num2 / num1;
                printk(KERN_INFO "Division: %d / %d = %d\n", (int)num2, (int)num1, (int)result);
            } else {
                printk(KERN_INFO "Cannot divide by zero\n");
            }
            break;
    }

    return len;
}

static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .write = keyboard_interrupt_write,
};

static int __init my_init(void) {
    int result;

    // Register the device
    result = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (result < 0) {
        printk(KERN_ERR "Failed to register character device\n");
        return result;
    }

    printk(KERN_INFO "Kernel module loaded successfully\n");

    return 0;
}

static void __exit my_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "Kernel module unloaded successfully\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishnuvardhan.k");
MODULE_DESCRIPTION("A simple kernel module to perform arithmetic operations on key press");
