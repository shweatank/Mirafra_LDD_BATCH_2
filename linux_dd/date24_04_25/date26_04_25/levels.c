#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A kernel module showing all printk levels");
MODULE_VERSION("1.0");

static int __init printk_levels_init(void)
{
    printk(KERN_EMERG   "KERN_EMERG: System is unusable\n");
    printk(KERN_ALERT   "KERN_ALERT: Immediate action required\n");
    printk(KERN_CRIT    "KERN_CRIT: Critical condition\n");
    printk(KERN_ERR     "KERN_ERR: Error condition\n");
    printk(KERN_WARNING "KERN_WARNING: Warning condition\n");
    printk(KERN_NOTICE  "KERN_NOTICE: Normal but significant\n");
    printk(KERN_INFO    "KERN_INFO: Informational message\n");
    printk(KERN_DEBUG   "KERN_DEBUG: Debug message\n");
    
    return 0;
}

static void __exit printk_levels_exit(void)
{
    printk(KERN_INFO "Exiting printk levels module\n");
}

module_init(printk_levels_init);
module_exit(printk_levels_exit);

