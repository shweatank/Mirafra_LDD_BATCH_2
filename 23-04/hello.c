#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

static int __init hello_init(void)
{
	printk(KERN_INFO "Hi");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "bye");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_VERSION("1.0");
MODULE_DESCRIPTION("SAMPLE MODULE");
MODULE_AUTHOR("VISHNU");
MODULE_LICENSE("GPL");
