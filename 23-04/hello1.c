#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>


static int __init hello_init(void)
{
	printk(KERN_INFO "hi");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "bye");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("simple code");
MODULE_VERSION("1.0");
MODULE_AUTHOR("VISHNU");
