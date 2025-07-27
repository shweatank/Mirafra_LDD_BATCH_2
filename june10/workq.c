#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/timer.h>
#include<linux/jiffies.h>
#include<linux/workqueue.h>
#include<linux/interrupt.h>
static struct timer_list tim;
static struct work_struct my_work;
char msg[]="vishnu";
static void my_timer_callback(struct timer_list *data)
{
	printk(KERN_INFO "timer interrupt:scheduling work queue..\n");
	schedule_work(&my_work);
	mod_timer(&tim,jiffies+msecs_to_jiffies(1000));
}

static int my_strlen(char *str)
{
	int i=0,c=0;
	while(str[i]!='\0')
	{
		i++;
		c++;
	}
	return c;
}
static void my_work_func( struct work_struct *work)
{
	int len=my_strlen(msg);
	int i,j;
	for(i=0,j=len-1;i<j;i++,j--)
	{
		char temp=msg[i];
		msg[i]=msg[j];
		msg[j]=temp;
	}
	printk(KERN_INFO "string=%s\n",msg);
}

static int __init hello_init(void)
{
	printk(KERN_INFO "workqueue is started...\n");
	INIT_WORK(&my_work,my_work_func);
	timer_setup(&tim,my_timer_callback,0);
	mod_timer(&tim,jiffies+msecs_to_jiffies(1000));
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "workqueue is exited...\n");
	del_timer(&tim);
	cancel_work_sync(&my_work);
}
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
