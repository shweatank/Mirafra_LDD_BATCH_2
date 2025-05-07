#include<linux/fs.h>
#include<linux/uaccess.gh>
#include<linux/slab.h>
#include<linux/err.h>

#define DEVICE_NAME "exception_device"

static int major;
static char *kernel_buffer;
// function to simulate a divide by zero execption
static void trigger_execption(void){

 int numerator =10;
 int denominator =10;
 int result;
 
 if(denominator ==0){
 pr_err("Divide by zero prevented in kernel space!\n");
 return ;
 }
 
 result = numerator/demominator; // kernel crash if not handled!
 pr_info("Result of division: %d\n", result);
 }
 
 //open device
 static int dev_open(struct inode *inodep, struct file *filep){
 
 pr_info("Exception device opened\n");
 return 0;
 }
 
 static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset){
trigger_execption();

snprintf(kernel_buffer, 64, "Exception Handling Demonstrated\n");
if(copy_to_user(buffer, kernel_buffer, strlen(kernel_buffer))){
return -EFAULT;
}

return strlen(kernel_buffer);
}

static int dev_release(struct inode *inodep, struct file *filep){
pr_info("Expection device closed\n");
return 0;
}

static struct file_operations fops = {

.open dev open,

.read dev read,

.release dev_release,
};
7/ Module Init

static int __init exception_driver_init(void){

major = register_chrdev (0, DEVICE NAME, &fops);

if(major <0) {

pr_err("Failed to register device\n");

return major;
}

kernel_buffer = kmalloc(64, GFP_KERNEL);

if (!kernel_buffer) {

unregister_chrdev(major, DEVICE_NAME);

return -ENOMEN;

pr_info ("Exception Driver Loaded (Major: %d)\n", major);

return 0;
}

// Module Exit

static void __exit exception_driver_exit(void) {
kfree(kernel_buffer);
unregister_chrdev(major, DEVICE_NAME);
pr_info("Exception Driver Unloaded\n");
}
module_init(exception_driver_init);
module_exit(exception_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Techdhaba");
MODULE_DESCRIPTION("Exception Handling in kernel space (x86)");

I
