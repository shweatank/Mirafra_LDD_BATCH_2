#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/timer.h>
#include<linux/jiffies.h>
#include<linux/interrupt.h>
static struct timer_list tim;
static unsigned long  count=0;

static struct tasklet_struct my_tasklet;

static void my_tasklet_func(unsigned long data)
{
	count++;
	printk(KERN_INFO "counter is %lu\n",count);
}

static void my_timer_callback(struct timer_list *data)
{
	printk(KERN_INFO "timer interrupt:scheduling tasklet..\n");
	tasklet_schedule(&my_tasklet);
	mod_timer(&tim,jiffies+msecs_to_jiffies(1000));
}

static int __init hello_init(void)
{
	printk(KERN_INFO "tasklet is inserted...\n");
	tasklet_init(&my_tasklet,my_tasklet_func,0);
	timer_setup(&tim,my_timer_callback,0);
	mod_timer(&tim,jiffies+msecs_to_jiffies(1000));
	return 0;
}
static void __exit hello_exit(void)
{
	del_timer(&tim);
	tasklet_kill(&my_tasklet);
	printk(KERN_INFO "tasklet is exited..\n");
	printk(KERN_INFO "final counter =%lu\n",count);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
