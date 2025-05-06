//Bottom Half Using Tasklets



#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#define SHARED_IRQ 1  // IRQ 1 is typically the keyboard

static int irq = SHARED_IRQ;
static int counter = 0;

// Tasklet function
static void my_tasklet_function(unsigned long data)
{
    counter++;
    printk(KERN_INFO "Tasklet executed: Counter = %d\n", counter);
}

// Declare and initialize tasklet
DECLARE_TASKLET(my_tasklet, my_tasklet_function, 0);

// Top half (IRQ handler)
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Top Half: Interrupt occurred, scheduling tasklet\n");
    tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;
}

// Module Init
static int __init tasklet_module_init(void)
{
    int ret;

    printk(KERN_INFO "Tasklet module loading...\n");

    ret = request_irq(irq, my_irq_handler, IRQF_SHARED, "my_tasklet_irq", &irq);
    if (ret) {
        printk(KERN_ERR "Unable to register IRQ %d\n", irq);
        return ret;
    }

    printk(KERN_INFO "IRQ %d registered successfully\n", irq);
    return 0;
}

// Module Exit
static void __exit tasklet_module_exit(void)
{
    tasklet_kill(&my_tasklet);
    free_irq(irq, &irq);
    printk(KERN_INFO "Tasklet module unloaded, IRQ %d freed\n", irq);
}

module_init(tasklet_module_init);
module_exit(tasklet_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohan");
MODULE_DESCRIPTION("Example of Bottom Half using Tasklet");
