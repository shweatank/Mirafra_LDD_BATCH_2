#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/device.h>

#define DEVICE "reverse"
#define buf_len 100
static int major;
char message[buf_len];
char a[buf_len];
static int dev_open(struct inode *inode,struct file* fp)
{
	printk(KERN_INFO "DEVICE is opened..\n");
	return 0;
}

static int dev_release(struct inode *inode, struct file* fp)
{
	printk(KERN_INFO "DEVICE is closed...\n");
	return 0;
}
static ssize_t dev_read(struct file* fp,char __user *buf,size_t len,loff_t * loff)
{
	char result_msg[buf_len];
	int msg_len=snprintf(result_msg,buf_len,"reverse;%s",a);
	int error_count=copy_to_user(buf,result_msg,msg_len);

	return error_count ==0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *fp,const char __user *buf,size_t len,loff_t * loff)
{
	int my_strlen(char *str)
{
        int c=0,i=0;
        while(str[i]!='\0')
        {
                i++;
                c++;
        }
        return c;
}

	if(copy_from_user(message,buf,len))
			return -EFAULT;
	message[len]='\0';
	if(sscanf(message,"%s",a)!=1)
	      return -EFAULT;
                  
	  int i,j;
	  int str_len=my_strlen(a);
	  for(i=0,j=str_len-1;i<j;i++,j--)
	  {
	  char temp=a[i];
	  a[i]=a[j];
	  a[j]=temp;
	  }

	  printk(KERN_INFO "reverse:%s\n",a);
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
		printk(KERN_INFO "ERROR");
		return 0;
	}

	printk(KERN_INFO "device registered with major number %d\n",major);
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO " device is un-registered..\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
