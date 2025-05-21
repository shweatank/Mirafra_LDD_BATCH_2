#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "charcalc"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static int result;

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "charcalc: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "charcalc: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char result_msg[BUF_LEN];
    int error_count;
    int msg_len;
    if(result == 0)
    {
	    error_count = copy_to_user(buffer,"invalid input ", 14);
		msg_len=14;
    }
    else
    {
   	 msg_len = snprintf(result_msg, BUF_LEN, "Result: %d\n", result);
     	 error_count = copy_to_user(buffer, result_msg, msg_len);
    }
    return error_count == 0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    int a,i;
    //char op;

    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';
    if (sscanf(message, "%d", &a) != 1) {
        printk(KERN_ERR "charcalc: Invalid input format\n");
        return -EINVAL;
    }
    result=0;
for(i=1;i<a;i++)
{
	
	if(i*i==a )
	{result=i;
		printk(KERN_INFO "charcalc: sqar of %d = %d\n", a,i);
		break;
	}
}
            printk(KERN_ERR "charcalc: Invalid data %d\n", a);
	   // return -EINVAL;
	
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init charcalc_init(void) 
{
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (imajor < 0) 
    {
        printk(KERN_ALERT "charcalc failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "charcalc: registered with major number %d\n", major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "charcalc: unregistered\n");
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("venkat");
MODULE_DESCRIPTION("A simple character device that performs + and -");
