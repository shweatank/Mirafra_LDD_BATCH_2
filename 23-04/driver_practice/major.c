#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>

dev_t dev=0;

static int __init major_init(void)
{
	if(alloc_chrdev_region(&dev,0,1,"vishnu")<0)
	{
		printk(KERN_INFO "failed");
		return 0;
	}
	printk(KERN_INFO "module inserted successfully...\n");
	return 0;
}

static void __exit major_exit(void)
{
	unregister_chrdev_region(dev,1);
	printk(KERN_INFO "module is deleted successfully....\n");
}

module_init(major_init);
module_exit(major_exit);

MODULE_VERSION("1.0");
MODULE_DESCRIPTION("MAJOR NUMBER");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHNU");
