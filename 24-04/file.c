#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

static int __init file_init(void)
{
	printk(KERN_INFO "hi");
	return 0;
}

static void __exit file_exit(void)
{
	printk(KERN_INFO "bye");
}

module_init(file_init);
module_exit(file_exit);

MODULE_LICENSE("GPL");

