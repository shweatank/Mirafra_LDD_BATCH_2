#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

void my_tasklet_function(struct tasklet_struct *t);

// Define a tasklet
DECLARE_TASKLET(my_tasklet, my_tasklet_function);

// Tasklet function
void my_tasklet_function(struct tasklet_struct *t) {
    printk(KERN_INFO "Tasklet executed!\n");
}

// Module initialization
static int __init my_module_init(void) {
    printk(KERN_INFO "Module loaded!\n");
    
    // Schedule the tasklet
    tasklet_schedule(&my_tasklet);
    
    return 0;
}

// Module cleanup
static void __exit my_module_exit(void) {
    // Kill the tasklet before exiting
    tasklet_kill(&my_tasklet);
    printk(KERN_INFO "Module unloaded!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishnuvardhan.K");
MODULE_DESCRIPTION("Example Tasklet Kernel Module");
