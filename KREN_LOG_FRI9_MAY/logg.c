#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#define DEVICE "logging"
static char *buffer=NULL;
static int major;
static size_t buf_len=1024;



static void concatenate(const char *src)
{
    size_t new_len = buf_len + strlen(src);
    buffer = krealloc(buffer, new_len + 1, GFP_KERNEL);
    
    if (buffer)
    {
        strcat(buffer, src);
        buf_len = new_len;
        printk(KERN_INFO "Updated buffer: %s\n", buffer);
    }
    else
    {
        printk(KERN_ERR "Memory reallocation failed!\n");
    }
}

static int dev_open(struct inode *in, struct file *fp){
printk(KERN_INFO "the device is opened\n");
return 0;
}
static int dev_release(struct inode *in,struct file *fp){
printk(KERN_INFO "the device is closed\n");
return 0;
}

static ssize_t dev_read(struct file *fp, char __user *buf, size_t len, loff_t *offset){
int error_count = copy_to_user(buf,buffer, strlen(buffer));
        memset(buffer,'\0',strlen(buf));
    return error_count == 0 ? len: -EFAULT;

}

static struct file_operations fops={
.owner=THIS_MODULE,
.open=dev_open,
.release=dev_release,
.read=dev_read
};
static int __init logging_init(void){
buffer = kmalloc(1024, GFP_KERNEL);
    if (!buffer) return -ENOMEM;
    buffer[0] = '\0';

    // Simulating kernel-generated messages being appended
    concatenate("Kernel module started. ");
    concatenate("Memory allocated successfully. ");
    concatenate("System initialized. ");
    
    printk(KERN_INFO "Kernel module loaded\n");

major=register_chrdev(0,DEVICE,&fops);
if(major<0){
printk(KERN_INFO "major number reg unsuccessful\n");
return major;
}
printk(KERN_INFO "major number is :%d",major);
    return 0;

}

static void __exit logging_exit(void){
unregister_chrdev(major,DEVICE);
kfree(buffer);
}

module_init(logging_init);
module_exit(logging_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("venkat");
MODULE_DESCRIPTION("this is simple logging code\n");

