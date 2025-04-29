#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "dup_dmesg"
static int major;
static char message[100];
static int dev_open(struct inode *in,struct file *filep){
printk(KERN_INFO "the device is opened");
return 0;
}
static int dev_release(struct inode *in, struct file *filep){
printk(KERN_INFO "device closed");
return 0;
}
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int error_count = copy_to_user(buffer,message, 100);

    return error_count == 0 ? len: -EFAULT;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    //.write = dev_write,
    .release = dev_release,
};



static int __init charcalc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    sprintf(message,"The device is registered");
    if (major < 0) {
        printk(KERN_ALERT "kernel_log failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "kernel log: registered with major number %d\n", major);
    sprintf(message," major number is created : %d",major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "kernel log: unregistered\n");
//    return 0;
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("venkat");
MODULE_DESCRIPTION("A simple character device that performs + and -");
                                                                                      
