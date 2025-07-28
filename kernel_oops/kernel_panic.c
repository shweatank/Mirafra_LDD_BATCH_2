//our system will panic and reboot (if kdump is configured)
#include <linux/module.h>
#include <linux/kernel.h>

static int __init panic_init(void)
{
    panic("💥 Intentional Kernel Panic for Testing!\n");
    return 0;
}

static void __exit panic_exit(void)
{
    printk(KERN_INFO "Panic module unloaded\n");
}

module_init(panic_init);
module_exit(panic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Simple module to trigger a kernel panic");

