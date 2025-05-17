#include <linux/module.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60



static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    printk(KERN_INFO "The keyboard pressed scancode 0x%02x \n", scancode);
    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    int result ;
    result = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, "keyboard_interrupt_handler", (void *)keyboard_interrupt_handler);
    if(result < 0)
    {
        printk(KERN_INFO "rquest_irq error");
        return 1;
    }

    printk(KERN_INFO, "Module loaded");
    return result;
}

static void __init my_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)keyboard_interrupt_handler); 
    printk(KERN_INFO "Module unloaded");
    
}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL");
