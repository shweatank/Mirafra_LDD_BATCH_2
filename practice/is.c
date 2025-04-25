#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/slab.h>

#define DEVICE "string_rev"

#define WR_VALUE _IOW(100,0, char *)
#define RD_VALUE _IOR(100,1,char *)

static int major;
char result[100];
char string[100];
static int my_strlen(char *str)
{
	int i=0,c=0;
	while(str[i]!='\0')
	{
		c++;
		i++;
	}
	return c;
}
static void operation(void)
{
	int i,j;
	int len=my_strlen(string);
	for(i=0,j=len-1;i<j;i++,j--)
	{
		char temp=string[i];
		string[i]=string[j];
		string[j]=temp;
	}
	for(i=0;i<len;i++)
	{
		result[i]=string[i];
	}
}

static long dev_ioctl(struct file *fp,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case WR_VALUE:
			       if(copy_from_user(string,(char __user*)arg,100))
				       return -EFAULT;
			       operation();
			       break;
		case RD_VALUE: if(copy_to_user((char __user*)arg,result,100))
				       return -EFAULT;
			       break;
	}
	return 0;
	
}

static int dev_open(struct inode *inode, struct file *fp)
{

	printk(KERN_INFO "device is opened\n");
	return 0;
}
static int dev_release(struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "device is closed\n");
	return 0;
}

static struct file_operations fops =
{
	.unlocked_ioctl = dev_ioctl,
	.open = dev_open,
	.release = dev_release,
};
static int __init hello_init(void)
{
	major=register_chrdev(0,DEVICE,&fops);
	if(major<0)
	{
		pr_info("ERROR\n");
	}

	printk(KERN_INFO "major number is %d\n",major);
	return 0;
}


static void __exit hello_exit(void)
{
	unregister_chrdev(major,DEVICE);
	printk(KERN_INFO " major number is unregistered");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
