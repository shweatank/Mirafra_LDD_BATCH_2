
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>  // for inb()
#include<linux/timer.h>
#include<Linux/jiffies.h>

#define KEYBOARD_IRQ 1     // Standard keyboard IRQ
#define KEYBOARD_DATA_PORT 0x60  // Data port for keyboard
static struct timer_list my_timer;
static my_timer_callback(struct timer_list *t)
{
printk(KERN_INFO "timer fired:%lu\n",jiffies);
}
// Interrupt handler function
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    // Read scancode from keyboard data port
    scancode = inb(KEYBOARD_DATA_PORT);

    printk(KERN_INFO "Keyboard interrupt: scancode = 0x%02x\n", scancode);

    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    int result;

    printk(KERN_INFO "Keyboard Interrupt Module with Scancode Reading Loaded\n");

    // Request IRQ for the keyboard
    result = request_irq(KEYBOARD_IRQ,
                         keyboard_interrupt_handler,
                         IRQF_SHARED,
                         "keyboard_interrupt_scancode",
                         (void *)(keyboard_interrupt_handler));
    timer_setup(&my_timer,my_timer_callback,0);
    mod_timer(&my_timer,jiffies+msecs_to_jiffies(2000));
    

    if (result) {
        printk(KERN_ERR "Failed to register keyboard IRQ\n");
        return result;
    }

    return 0;
}

static void __exit my_exit(void)
{
    del_timer_sync(&my_timer);
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard Interrupt Module with Scancode Reading Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard Interrupt Handler Reading Scancode Example");
