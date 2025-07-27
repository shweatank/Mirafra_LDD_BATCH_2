#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define TIMER_INTERVAL_MS 2000  

static struct timer_list my_timer;

static void timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "timer_interrupt: Timer callback called at jiffies = %ld\n", jiffies);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));
}

static int __init timer_init(void)
{
    printk(KERN_INFO "timer_interrupt: Module loaded. Initializing timer...\n");

    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_INTERVAL_MS));

    return 0;
}

static void __exit timer_exit(void)
{
    del_timer(&my_timer);
    printk(KERN_INFO "timer_interrupt: Module unloaded. Timer stopped.\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEJA");
MODULE_DESCRIPTION("Kernel Timer Interrupt Example");

