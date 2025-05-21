#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tasklet");
MODULE_DESCRIPTION("Tasklet Project");

static unsigned int counter = 0;


static void my_tasklet_function(struct tasklet_struct *t);


static DECLARE_TASKLET(my_tasklet, my_tasklet_function);

static void my_tasklet_function(struct tasklet_struct *t)
{
    counter++;
    printk(KERN_INFO "Tasklet executed! Counter = %u\n", counter);
}

static void interrupt(void)
{
    printk(KERN_INFO "Interrupt occurred. Scheduling tasklet...\n");
    tasklet_schedule(&my_tasklet);
}

static int __init my_module_init(void)
{
    printk(KERN_INFO "Tasklet module loaded.\n");
    interrupt(); 
    return 0;
}


static void __exit my_module_exit(void)
{
    tasklet_kill(&my_tasklet); 
    printk(KERN_INFO "Tasklet module unloaded.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

