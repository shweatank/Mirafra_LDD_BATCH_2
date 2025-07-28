// leak_module.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static int __init leak_init(void)
{
    void *leak_ptr;

    pr_info("Leak Module Loaded\n");

    leak_ptr = kmalloc(1024, GFP_KERNEL);  // Allocate 1KB
    if (!leak_ptr)
        return -ENOMEM;

    // Intentionally do not free `leak_ptr`
    pr_info("Leaked 1KB at %p\n", leak_ptr);

    return 0;
}

static void __exit leak_exit(void)
{
    pr_info("Leak Module Unloaded\n");
}

module_init(leak_init);
module_exit(leak_exit);

