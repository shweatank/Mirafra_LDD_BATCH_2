#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init panic_module_init(void)
{
    pr_info("Triggering kernel panic...\n");
    panic("Manual kernel panic for testing kdump\n");
    return 0;
}

static void __exit panic_module_exit(void)
{
    pr_info("Panic module exit (never reached).\n");
}

module_init(panic_module_init);
module_exit(panic_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GANGA");
MODULE_DESCRIPTION("Module to trigger kernel panic for testing.");

