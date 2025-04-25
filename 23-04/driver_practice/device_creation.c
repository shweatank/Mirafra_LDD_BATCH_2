#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
dev_t dev=0;
static struct class *dev_class;

static int __init device_init(void)
{
	if(alloc_chrdev_region(&dev,0,1,"device")<0)
		{
			printk(KERN_INFO "failed");
		}
	dev_class=class_create("class");

	if(device_create(dev_class,NULL,dev,NULL,"vishnu")<0)
	{
		printk(KERN_INFO "failed");
	}
	printk(KERN_INFO "Device is created successfully...\n");
	return 0;
}

static void __exit device_exit(void)
{
	class_destroy(dev_class);
	device_destroy(dev_class,dev);
	unregister_chrdev_region(dev,1);
	printk(KERN_INFO "device is removed successfully...\n");
}
module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VISHNU");
MODULE_DESCRIPTION("DEVICE CREATION");
MODULE_VERSION("1.0");
