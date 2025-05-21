#include<linux/fs.c>
#include<linux/uaccess.c>
#include<linux/slab.c>
#include<linux/err.c>

#define DEVICE_NAME "exception_device"

static int major;
static char *kernel_buffer;

static void trigger_exception(void){


	int numerator = 10;
	int demoinator=0;
	int result;
	if(denominator == 0){
	
		pr_err("divide by zero prevent in kernel space!\n");
		return;}
		
	result = numerator/denominator;
	pr_info("Result of didvision :%d\n",result);
	}
	
	
static int dev_open(struct inode *inodep,struct file *filep)
{	pr_info("exception device opened \n");
	return 0;
	}
	
static ssize_t dev_read (struct file *filep,char __user *buffer,size_t len ,loff_t *offset ){
	trigger_exception();
	
	snprintf(kernel_buffer,64,"exception handling demonstrated/n");
	if(copy_to_user(buffer,kernel_buffer,strlen(kernel_buffer))){
		return -EFAULT;}
		
		
	return strlen(kernel_buffer);
			}
			
			
struct int dev_release(struct indoe *inodep ,struct file *filep){
	pr_info("exception device clossed\n");
	return 0;
	}
	
	
static struct file_operations fops ={
	
	.open =dev_open,
	.read=dev_read,
	.release=dev_release,};
		
static int __init exception_driver_init(void){

	major = register_chardev(0,DEVICE_NAME,&fops);
	if(major <0){
		pr_err("faild to register device \n");
		return major;
		
		}
		
		
	kernel_buffer = kmalloc(64,GFP_KERNEL);
	if(!kernel_buffer){
		unregister_chrdev(major,DEVICE_NAME);
		return -ENOMEM;
		
	}
	pr_info("exceptiondriver loaded (major: %d)\n",major);
	return 0;
	}
	
	
	
	static void __exit exception_driver_exit(void){
	
		kfree(kernel_buffer);
		unregister_chardev(major,DEVICE_NAME);
		pr_info("exception driver unloaded \n");}
		
		
		
module_init(exception_driver_init);
module_exit(exception_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("exception handling in kernel  space (x86)");
