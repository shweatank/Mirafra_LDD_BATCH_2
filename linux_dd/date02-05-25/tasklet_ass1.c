#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>

static int irq = 1;  // Default IRQ (e.g., keyboard IRQ on x86; adjust if needed)
static int counter = 0;

// Tasklet function
static void my_tasklet_func(struct tasklet_struct *t)
{
    counter++;
    printk(KERN_INFO "Tasklet executed! Counter = %d\n", counter);
}

// Declare tasklet
DECLARE_TASKLET(my_tasklet, my_tasklet_func);

// Interrupt handler (top half)
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Interrupt received: scheduling tasklet\n");
    tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;
}

static int __init my_module_init(void)
{
    int ret;
    printk(KERN_INFO "Tasklet module loading\n");

    ret = request_irq(irq, my_irq_handler, IRQF_SHARED, "my_tasklet_irq", (void *)&irq);
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", irq);
        return ret;
    }

    printk(KERN_INFO "IRQ %d registered. Trigger interrupt to test.\n", irq);
    return 0;
}

static void __exit my_module_exit(void)
{
    free_irq(irq, (void *)&irq);
    tasklet_kill(&my_tasklet);
    printk(KERN_INFO "Tasklet module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Example of tasklet (bottom half) handling an interrupt");

