#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
int count=0;
struct calc_data
{
	int num1;
	int num2;
};
#define DEVICE_NAME "/dev/simple_calculator"

#define MAGIC_NUM 100

#define KEYBOARD_IRQ 1  // Keyboard IRQ number
#define IOCTL_SET_DATA_LOW(MAGIC_NUM,O, calc_data)
// Interrupt handler function
static struct calc_data calc_data;
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
	int sum;
	sum=calc_data.num1+calc_data.num2;
    //printk(KERN_INFO "Keyboard interrupt received !\n");
    
	printk("count=%d\n",sum);
    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    int result;

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

    return 0;
}

static void __exit my_exit(void)
{
    // Free the IRQ
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard Interrupt Module Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple Linux Keyboard Interrupt Handler Example");
