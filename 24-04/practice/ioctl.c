#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/ioctl.h>

#define DEVICE "calc"
#define WR_VALUE _IOW(100,0,struct cal_buf *)
#define RD_VALUE _IOR(100,1,int *)

static int major;
struct cal_buf
{
	int num1;
	int num2;
	char operation;
	int result;
};

struct cal_buf *calc;

static void operation(void)
{

	
	switch(calc->operation)
	{
		case '+' : calc->result=calc->num1+ calc->num2;break;
		case '-' : calc->result=calc->num1-calc->num2;break;
		case '*' : calc->result=calc->num1*calc->num2; break;
		default : calc->result=0;
	}
}
static long dev_ioctl(struct file *fp,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case WR_VALUE:
			if(copy_from_user(calc,(struct cal_buf __user *)arg,sizeof(struct cal_buf)))
				return -EFAULT;
			operation();
			break;
		case RD_VALUE:
			if(copy_to_user((int __user *)arg,&calc->result,sizeof(int)))
				return -EFAULT;
			break;

		default:
            			printk(KERN_INFO "default");
	}
	return 0;
}
static int dev_open(struct inode *inode,struct file *fp)
{
	calc=kmalloc(sizeof(struct cal_buf),GFP_KERNEL);
	printk(KERN_INFO "device opened...\n");
	return 0;
}

static int dev_release(struct inode *inode, struct file *fp)
{
	if(calc)
	{
		kfree(calc);
	}
	printk(KERN_INFO "device close..\n");
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
	if(major<0)
	{
		printk(KERN_INFO "major failed..\n");
	}
	printk(KERN_INFO "major number register with %d\n",major);
	return 0;
}

static void __exit hello_exit(void)
{
	unregister_chrdev(major,DEVICE);
	printk(KERN_INFO "major is unregistered.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");


