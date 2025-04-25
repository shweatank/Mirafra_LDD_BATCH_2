#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>

#define DEVICE "vishnu"

#define buf_len 100

static int result;
char message[buf_len];
static int major;

static int dev_open(struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "device is opened..\n");
	return 0;
}
static int dev_release(struct inode *inode,struct file *fp)
{
	printk(KERN_INFO "device is closed..\n");
	return 0;
}

static ssize_t dev_read(struct file *fp,char __user *buf,size_t len, loff_t * off)
{
	char result_msg[buf_len];
	int msg_len=snprintf(result_msg,buf_len,"result=%d\n",result);
	int error_count=copy_to_user(buf,result_msg,msg_len);
	return error_count ==0 ? msg_len :-EFAULT;
}
static ssize_t dev_write(struct file *fp,const char __user *buf, size_t len,loff_t * off)
{
	int a;
	if(copy_from_user(message,buf,len))
		return -EFAULT;
	message[len] ='\0';
	if(sscanf(message,"%d",&a)!=1)
	{
		printk(KERN_INFO "invalid arguments\n");
		return 0;
	}

	result=a*a;
	printk(KERN_INFO "Result=%d\n",result);
	return len;
}

static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open  = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

static int __init hello_init(void)
{
	major=register_chrdev(0,DEVICE,&fops);
	printk(KERN_INFO "device is registered with major number%d\n",major);
	return 0;
}

static void __exit hello_exit(void)
{
	unregister_chrdev(major,DEVICE);
	printk(KERN_INFO "device is unregistered..\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
