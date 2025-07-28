// dereference.c
#include <linux/module.h>
#include <linux/kernel.h>

int *ptr = NULL;

static int __init oops_init(void)
{
    printk(KERN_INFO "Triggering Oops by dereferencing NULL pointer...\n");
    printk(KERN_INFO "Oops: %d\n", *ptr);  // NULL dereference
    return 0;
}

static void __exit my_oops_exit(void)
{
    printk(KERN_INFO "Oops module exit\n");
}

module_init(oops_init);
module_exit(my_oops_exit);
MODULE_LICENSE("GPL");

