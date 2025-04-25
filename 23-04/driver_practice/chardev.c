#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>

dev_t dev=0;

static struct cdev v;

static int v_open(struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "file is opened\n");
	return 0;
}

static int v_release(struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "file is close\n");
	return 0;
}

static ssize_t v_read(struct file *fd,char __user *buf, size_t len, loff_t* off)
{
	printk(KERN_INFO "contents are read\n");
	return 0;
}

static ssize_t v_write(struct file *fd,const char __user *buf,size_t len, loff_t* off)
{
	printk(KERN_INFO "contents are write..\n");
	return 0;
}

static struct file_operations fops =
{
	.owner=THIS_MODULE,
	.open=v_open,
	.release=v_release,
	.read=v_read,
	.write=v_write,
};

static int __init cdev1_init(void)
{
	cdev_init(&v,&fops);
	if(cdev_add(&v,dev,1)<0)
	{
		printk(KERN_INFO "failed");
	}
	printk(KERN_INFO "character device is added successfully\n");
	return 0;
}
static void __exit cdev1_exit(void)
{
	cdev_del(&v);
	printk(KERN_INFO "character device is removed successfully..\n");
}

module_init(cdev1_init);
module_exit(cdev1_exit);

MODULE_LICENSE("GPL");

