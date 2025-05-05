/*Bottom Half Using Tasklets

Implement a tasklet that processes some dummy data (e.g., increment a counter) whenever an interrupt occurs.

Log messages using printk to show tasklet execution.*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>

#define IRQ_NO 1

static int counter = 0;
static struct tasklet_struct my_tasklet;

void my_tasklet_function(struct tasklet_struct *t)
{
    counter++;
    printk(KERN_INFO "Tasklet executed: counter = %d\n", counter);
}

static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Interrupt occurred: scheduling tasklet\n");
    tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;
}

static int __init my_module_init(void)
{
    int ret;

    printk(KERN_INFO "Loading tasklet module...\n");
    tasklet_setup(&my_tasklet, my_tasklet_function);

    ret = request_irq(IRQ_NO, my_irq_handler, IRQF_SHARED, "my_tasklet_irq", (void *)(my_irq_handler));
    if (ret) {
        printk(KERN_ERR "Failed to register IRQ %d\n", IRQ_NO);
        return ret;
    }

    printk(KERN_INFO "Tasklet module loaded. IRQ handler registered.\n");
    return 0;
}

static void __exit my_module_exit(void)
{
    tasklet_kill(&my_tasklet);
    free_irq(IRQ_NO, (void *)(my_irq_handler));
    printk(KERN_INFO "Tasklet module unloaded.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishnuvardhan.K");
MODULE_DESCRIPTION("Tasklet Example: Bottom Half Using Interrupts");

