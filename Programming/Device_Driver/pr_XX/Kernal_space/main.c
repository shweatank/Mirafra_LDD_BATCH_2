#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init log_demo_init(void)
{
    pr_emerg("pr_emerg: System is unusable!\n");
    pr_alert("pr_alert: Immediate action required!\n");
    pr_crit("pr_crit: Critical conditions!\n");
    pr_err("pr_err: Error occurred!\n");
    pr_warn("pr_warn (same as warning): Just a warning!\n");
    pr_notice("pr_notice: Normal but significant event.\n");
    pr_info("pr_info: Informational message.\n");
    pr_cont("pr_cont: Continued line...\n");
    pr_debug("pr_debug: Debugging info (won't show unless enabled).\n");

    return 0;
}

static void __exit log_demo_exit(void)
{
    pr_info("Exiting log_demo module.\n");
}

module_init(log_demo_init);
module_exit(log_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GPT");
MODULE_DESCRIPTION("Kernel module demonstrating pr_* logging macros");

