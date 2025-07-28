#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>

static void *leak;

static int __init kmemleak_test_init(void)
{
    leak = kmalloc(1024, GFP_KERNEL);
    pr_info("Allocated memory at %px\n", leak);
    return 0;
}

static void __exit kmemleak_test_exit(void)
{
    pr_info("Exiting kmemleak test module (leak not freed)\n");
    // Don't free the memory
}

module_init(kmemleak_test_init);
module_exit(kmemleak_test_exit);

MODULE_LICENSE("GPL");

