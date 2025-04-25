#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#define DEVICE_NAME "charcalc"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static int result;

static int dev_open(struct inode *inodep,struct file *filep)
{
	printk(KERN_INFO "charcalc:device opened\n");
	return 0;
}
static int dev_release(struct inode *inodep,struct file *filep)
{
	printk(KERN_INFO "charcalc:device closed\n");
	return 0;
}
static ssize_t dev_read(struct file *filep,char __uasr *buffer,size_t len,loff_t*offset)
{
	char result_msg[BUFF_LEN];
	int msg_len=snprintf(result_msg,BUFF_LEN,"result:%d\n",result);
	int error_count=copy_to_user(buffer,result_msg,msg_len);
	return error_count == 0? msg_len :-EFAULT;
}
