#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

#define KEYBOARD_IRQ 1
#define KEYBOARD_PORT 0x60

static int counter = 0;

static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id) {
    unsigned char scancode;
    scancode = inb(KEYBOARD_PORT);
    counter++;
    printk(KERN_INFO "Keyboard interrupt! Scancode: 0x%02x, Counter: %d\n", scancode, counter);
    return IRQ_HANDLED;
}

static int __init my_init(void) {
    int result;
    result = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, "keyboard_interrupt", (void *)(keyboard_interrupt_handler));
    if (result) {
        printk(KERN_ERR "Unable to register IRQ handler\n");
        return result;
    }
    printk(KERN_INFO "Keyboard interrupt handler registered successfully\n");
    return 0;
}

static void __exit my_exit(void) {
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard interrupt handler unregistered\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishnuvardhan.k");
MODULE_DESCRIPTION("A simple kernel module that counts keyboard interrupts");

