#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "exception_device"

static int major;
static char *kernel_buffer;

// function to simulate a divide by zero
static void trigger_exception(void) {
    int numerator = 10;
    int denominator = 0;
    int result;

    if(denominator == 0) {
	pr_err("divide by zero prevent in kernel space!\n");
	return;
    }

    result = numerator / denominator;

    pr_info("Result of dividion : %d\n",result);
}



static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("Exception Device opened\n");
    return 0;
}
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    trigger_exception();

    snprintf(kernel_buffer, 64, "Exception Handling Demponstrated\n");

    if( copy_to_user(buffer,kernel_buffer, strlen(kernel_buffer))) {
	return -EFAULT;
    }

    return strlen(kernel_buffer);


}
static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("Exception Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    
};

static int __init exception_module_init(void)
{
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
    
    pr_info("ExceptionDev registered: major number %d\n", major);

    return 0;
}

static void __exit exception_module_exit(void)
{
    kfree(kernel_buffer);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Exception  module unloaded\n");
}

module_init(exception_module_init);
module_exit(exception_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Exception Handling Device Driver Example");
