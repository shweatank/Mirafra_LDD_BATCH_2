// oops_test.c
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xyz");
MODULE_DESCRIPTION("Simple driver to trigger kernel oops");

static int __init oops_test_init(void)
{
    printk(KERN_INFO "Oops test module loaded\n");

    // Intentionally cause a NULL pointer dereference
    int *ptr = NULL;
    printk(KERN_INFO "Dereferencing NULL pointer now...\n");
    *ptr = 42;  // This will trigger a kernel oops

    return 0;
}

static void __exit oops_test_exit(void)
{
    printk(KERN_INFO "Oops test module unloaded\n");
}

module_init(oops_test_init);
module_exit(oops_test_exit);

