#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/err.h>
#include<linux/module.h>
#define DEVICE_NAME "exception_device"

static int major;
static char *kernel_buffer;

//Function to simulate a divide-by-zero exception
static void trigger_exception(void)
{
	int numerator = 10;
	int denominator = 0;
	int result;

      if(denominator == 0)
      {
	      pr_err("Divide by zero prevented in kernel space!\n");
	      return;
      }

      result = numerator/denominator;
      pr_info("Result of diision: %d\n",result);
}
//open device
static int dev_open(struct inode *inodep,struct file *filep)
{
	pr_info("Exception device opened\n");
	return 0;
}

//Read from device (trigger exception)
static ssize_t dev_read(struct file *filep,char __user *buffer,size_t len,loff_t *offset)
{
	trigger_exception();

	snprintf(kernel_buffer,64, "Exception Handling Demonstrated\n");
	if(copy_to_user(buffer,kernel_buffer,strlen(kernel_buffer)))
	{
		return -EFAULT;
	}
	return strlen(kernel_buffer);
}

//Close Device
static int dev_release(struct inode *inodep,struct file *filep)
{
	pr_info("Exception device closed");
	return 0;
}

//File operations
static struct file_operations fops = {
	.open = dev_open,
	.read = dev_read,
	.release = dev_release,
};

// Module init
static int  __init exception_driver_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register device\n");
        return major;
    }
    kernel_buffer = kmalloc(64,GFP_KERNEL);
    if(!kernel_buffer){
	    unregister_chrdev(major,DEVICE_NAME);
	    return -ENOMEM;
    }
    pr_info("Exception Driver  Loaded (Major: %d)\n", major);
    return 0;
}

// Module exit
static void __exit exception_driver_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Exception Driver Unloaded\n");
}

module_init(exception_driver_init);
module_exit(exception_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Exception Handling in Kernel space(x86)");
