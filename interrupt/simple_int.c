#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>

#define KEYBOARD_IRQ 1  // Keyboard IRQ number

int count;          //Getting old values with count increment, so that every i run the code it is increasing its value, declare it in init function

// Interrupt handler function
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
		count++;
    //printk(KERN_INFO "Keyboard interrupt received! %d\n",count);
    return IRQ_HANDLED;
}

//printk(KERN_INFO "Keyboard interrupt received! %d\n",count);
static int __init my_init(void)
{
    int result;

	count=0;

    printk(KERN_INFO "Keyboard Interrupt Module Loaded\n");

    // Request IRQ for the keyboard
    result = request_irq(KEYBOARD_IRQ,            // IRQ number
                         keyboard_interrupt_handler, // Interrupt handler
                         IRQF_SHARED,                // Shared IRQ flag
                         "keyboard_interrupt",       // Name
                         (void *)(keyboard_interrupt_handler)); // Device ID for sharing

    if (result) {
        printk(KERN_ERR "Failed to register keyboard IRQ\n");
        return result;
    }
//	printk(KERN_INFO "key rec %d\n",count);

    return 0;
}

static void __exit my_exit(void)
{
	//printk(KERN_INFO "key rec %d\n",count);	
		// Free the IRQ
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard Interrupt Module Unloaded\n");
	printk(KERN_INFO "ker rec %d\n",count);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple Linux Keyboard Interrupt Handler Example");
