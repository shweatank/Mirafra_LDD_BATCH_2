#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Keerthi");
MODULE_DESCRIPTION("Module to trigger a kernel oops");

static int __init trigger_oops_init(void)
{
    printk(KERN_INFO "Loading oops module...\n");

    int *ptr = NULL;

    printk(KERN_INFO "About to dereference NULL pointer...\n");

    // This causes a kernel oops
    int val = *ptr;

    printk(KERN_INFO "This will not be printed. Value: %d\n", val);
    return 0;
}

static void __exit trigger_oops_exit(void)
{
    printk(KERN_INFO "Oops module unloaded\n");
}

module_init(trigger_oops_init);
module_exit(trigger_oops_exit);

