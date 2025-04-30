#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#define WR_VALUE _IOW(100,0,struct calc_buf *)
#define RD_VALUE _IOR(100,1,int *)

#define DEVICE "ioctl_calc"
struct calc_buf
{
	int num1;
	int num2;
	char operator;
	int result;
};

static int major;
struct calc_buf *cal;
static void operation(void)
{
	switch(cal->operator)
	{
		case '+' : cal->result=cal->num1+cal->num2; break;
		case '-' : cal->result=cal->num1-cal->num2; break;
		case '*' : cal->result=cal->num1*cal->num2; break;
		case '/' : if(cal->num2!=0)
				   cal->result=cal->num1/cal->num2;
			   else
			         cal->result=0;
				   break;
		default : cal->result=0;
	}
}
static int dev_open(struct inode *inode, struct file *fp)
{
	cal=kmalloc(sizeof(struct calc_buf),GFP_KERNEL);
	printk(KERN_INFO "device is opened..\n");
	return 0;
}

static int dev_release(struct inode *inode,struct file *fp)
{
	if(cal)
	{
		kfree(cal);
	}
	printk(KERN_INFO " device is closed...\n");
	return 0;
}

static long dev_ioctl(struct file *fp,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case WR_VALUE:if(copy_from_user(cal,(struct calc_buf __user *)arg,sizeof(struct calc_buf)))
				      return -EFAULT;
			      operation();
			      break;
		case RD_VALUE: if(copy_to_user((int  __user *)arg,&cal->result,sizeof(int)))
				       return -EFAULT;
			       break;
	}
	return 0;
}

static struct file_operations fops =
{
	.unlocked_ioctl=dev_ioctl,
	.open=dev_open,
	.release=dev_release,
};

static int __init hello_init(void)
{
	major=register_chrdev(0,DEVICE,&fops);
	if(major <0)
	{
		printk(KERN_INFO "major failed\n");
		return 0;
	}
	printk(KERN_INFO "major is registed with %d\n",major);
	return 0;
}

static void __exit hello_exit(void)
{
	unregister_chrdev(major,DEVICE);
	printk(KERN_INFO "major is unregistered successfully");
}
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
