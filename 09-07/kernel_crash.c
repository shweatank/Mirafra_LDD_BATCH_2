#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Keerthi");
MODULE_DESCRIPTION("Module to trigger a kernel oops");
static int __init trigger_oops_init(void){
	printk(KERN_INFO "Loading oops module...\n");
	int a=8,b=0;
	int x=a/b;
	printk(KERN_INFO "the kerenel crash:%d",x);
	return 0;
}

static void __exit trigger_oops_exit(void)
{
	printk(KERN_INFO "Oops module unloaded\n");
}

module_init(trigger_oops_init);
module_exit(trigger_oops_exit);


