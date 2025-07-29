#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>

static char *leak_ptr;

static int __init leak_init(void)
{
    leak_ptr = kmalloc(1024, GFP_KERNEL);  // Allocate memory and DON'T free
    if (leak_ptr)
        pr_info("Memory allocated at %px\n", leak_ptr);
    return 0;
}

static void __exit leak_exit(void)
{
    // Memory leak happens if we don't free this
    // kfree(leak_ptr); // UNCOMMENT this line to fix the leak
    pr_info("Module exiting\n");
}

module_init(leak_init);
module_exit(leak_exit);

MODULE_LICENSE("GPL");

