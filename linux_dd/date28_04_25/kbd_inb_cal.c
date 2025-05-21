#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>  // for inb()

#define KEYBOARD_IRQ 1           // Standard keyboard IRQ
#define KEYBOARD_DATA_PORT 0x60  // Keyboard data port

static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;
    int num1 = 5, num2 = 6;
    int result = 0;

    // Read scancode from keyboard data port
    scancode = inb(KEYBOARD_DATA_PORT);

    // Print scancode for debugging
    printk(KERN_INFO "Raw scancode: 0x%02x\n", scancode);

    // Ignore break codes (key release)
    if (scancode & 0x80)
        return IRQ_HANDLED;

    // Process make codes (key press only)
    switch (scancode) {
        case 0x1c:  // 'a' key
            result = num1 + num2;
            printk(KERN_INFO "ADD: %d = %d + %d\n", result, num1, num2);
            break;
        case 0x32:  // 'b' key
            result = num1 - num2;
            printk(KERN_INFO "SUB: %d = %d - %d\n", result, num1, num2);
            break;
        case 0x21:  // 'c' key
            result = num1 * num2;
            printk(KERN_INFO "MUL: %d = %d * %d\n", result, num1, num2);
            break;
        case 0x23:  // 'd' key
            if (num2 != 0) {
                result = num1 / num2;
                printk(KERN_INFO "DIV: %d = %d / %d\n", result, num1, num2);
            } else {
                printk(KERN_INFO "DIV: Division by zero!\n");
            }
            break;
        default:
            printk(KERN_INFO "Unrecognized key: 0x%02x\n", scancode);
    }

    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    int result;

    printk(KERN_INFO "Keyboard Interrupt Module Loaded\n");

    result = request_irq(KEYBOARD_IRQ,
                         keyboard_interrupt_handler,
                         IRQF_SHARED,
                         "keyboard_interrupt",
                         (void *)(keyboard_interrupt_handler));

    if (result) {
        printk(KERN_ERR "Failed to request IRQ\n");
        return result;
    }

    return 0;
}

static void __exit my_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard Interrupt Module Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Keyboard IRQ handler for arithmetic using scancodes");

