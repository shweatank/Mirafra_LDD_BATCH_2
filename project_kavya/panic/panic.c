#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init crash_trigger_init(void)
{
    printk(KERN_INFO "Loading crash trigger module...\n");

    int a = 1;
    int b = 0;
    int c;

    printk(KERN_INFO "About to divide by zero...\n");
    c = a / b;  // 🔥 This will cause kernel panic (divide error)

    printk(KERN_INFO "Result: %d\n", c);
    return 0;
}

static void __exit crash_trigger_exit(void)
{
    printk(KERN_INFO "Crash module unloaded\n");
}

module_init(crash_trigger_init);
module_exit(crash_trigger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kavya");
MODULE_DESCRIPTION("Kernel panic test module (division by zero)");
