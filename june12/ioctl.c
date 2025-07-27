#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/ioctl.h>

#define DEVICE_NAME "simple_calculator"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM,0,struct calc_data*)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM,1,int *)

MODULE_LICENSE("GPL");


struct calc_data
{
	int num1;
	int num2;
	char operation;
	int result;
};

static int major;
static struct calc_data *calc_buffer;

static void perform_calculation(void)
{
	switch(calc_buffer->operation)
	{
		case '+' : calc_buffer->result = calc_buffer->num1 + calc_buffer->num2; break;
	 	case '-' : calc_buffer->result = calc_buffer->num1 - calc_buffer->num2; break;
 		case '*' : calc_buffer->result = calc_buffer->num1 * calc_buffer->num2; break;
		case '/' : if(calc_buffer->num2 != 0)
			   calc_buffer->result = calc_buffer->num1 / calc_buffer->num2; 
			   else
				   calc_buffer->result=0;
			   break;
		default:calc_buffer->result=0;
	}
}


static long dev_ioctl(struct file *fp,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case IOCTL_SET_DATA:
			if(copy_from_user(calc_buffer,(struct calc_data __user *)arg,sizeof(struct calc_data)))
				return -EFAULT;
			perform_calculation();
			break;
		case IOCTL_GET_RESULT:
			if(copy_to_user((int __user *)arg,&calc_buffer->result,sizeof(int)))
				return -EFAULT;
			break;
	}
}

static int dev_open(struct inode *inode, struct file *filep)
{
	calc_buffer=kmalloc(sizeof(struct calc_data),GFP_KERNEL);
	pr_info("device opened\n");
	return 0;
}

static int dev_release(struct inode *inode, struct file *filep)
{if(calc_buffer)
	{
	kfree( calc_buffer);
	}
	pr_info("device closed..\n");
	return 0;
}
static struct file_operations fops = {
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

// Module init
static int __init simple_calculator_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register device\n");
        return major;
    }
    pr_info("Simple Calculator Loaded (Major: %d)\n", major);
    return 0;
}

// Module exit
static void __exit simple_calculator_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Simple Calculator Unloaded\n");
}

module_init(simple_calculator_init);
module_exit(simple_calculator_exit);
			  

