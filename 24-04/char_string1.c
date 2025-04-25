#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/uaccess.h>

#define DEVICE "string_rev"


static int major;
char message[100];
char a[100];
char result[100];
static int my_strlen(char *str)
{
	int i=0,c=0;
	while(str[i]!='\0')
	{
		i++;
		c++;
	}
	return c;
}
static int dev_open(struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "device is opened...\n");
	return 0;
}

static int dev_release(struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "device is closed...\n");
	return 0;
}

static ssize_t dev_read(struct file *fp,char __user *buf,size_t len,loff_t * off)
{
	char result_msg[100];
	int msg_len=snprintf(result_msg,100,"reverse=%s",result);
	int count=copy_to_user(buf,result_msg,msg_len);
	return count == 0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *fp,const char __user *buf,size_t len, loff_t * off)
{
	int i,j;
	if(copy_from_user(message,buf,len))
		return -EFAULT;
	message[len]='\0';
	if(sscanf(message,"%c",a)>100)
	{
		printk(KERN_INFO "Invalid ");
		return -EFAULT;
	}
	int len1=my_strlen(message);
	for(i=0,j=len1-1;i<j;i++,j--)
	{
		char temp=a[i];
		a[i]=a[j];
		a[j]=temp;
	}
	for(i=0;i<len;i++)
	{

		result[i]=a[i];
	}
	printk(KERN_INFO "Reverse string:%s\n",result);
		return len;
}
static struct file_operations fops =
{
	.owner=THIS_MODULE,
	.open=dev_open,
	.read=dev_read,
	.write=dev_write,
	.release=dev_release,
};

static int __init hello_init(void)
{
	major=register_chrdev(0,DEVICE,&fops);
         if(major<0)
	 {
		 printk(KERN_INFO "major is failed\n");
		 return major;
	 }
	printk(KERN_INFO "major number is register %d\n",major);
	return 0;
}

static void __exit hello_exit(void)
{
	unregister_chrdev(major,DEVICE);
	printk(KERN_INFO "major number is unregistered..\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
