#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list my_timer;

static void my_timer_callback(struct timer_list *t)
{
    pr_info("Periodic Timer: jiffies = %lu\n", jiffies);

    // Restart timer to fire again after 1 sec
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
}

static int __init periodic_timer_init(void)
{
    pr_info("Initializing periodic timer\n");

    timer_setup(&my_timer, my_timer_callback, 0);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    return 0;
}

static void __exit periodic_timer_exit(void)
{
    del_timer_sync(&my_timer);
    pr_info("Exiting periodic timer\n");
}

module_init(periodic_timer_init);
module_exit(periodic_timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Periodic Linux Timer Example");
