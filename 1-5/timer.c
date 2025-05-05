#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/time.h>
#include<linux/interrupt.h>
#include<linux/jiffies.h>

static struct timer_list tim;
static int count =0;

static void timer_callback(struct timer_list *data)
{
	pr_info("timer callback is called %d\n",count++);

	mod_timer(&tim,jiffies+msecs_to_jiffies(5000));
}
static int __init hello_init(void)
{
	timer_setup(&tim,timer_callback,0);

	mod_timer(&tim,jiffies+msecs_to_jiffies(5000));
	pr_info("timer insertion is done..\n");
	return 0;
}

static void __exit hello_exit(void)
{
	del_timer(&tim);
	pr_info("timer is deleted..\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
