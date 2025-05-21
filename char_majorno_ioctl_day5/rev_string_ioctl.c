#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "string_rev"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, char *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, char *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VRK");
MODULE_DESCRIPTION("Kernel string rev");
MODULE_VERSION("1.0");

static char arr[100];
static int major;

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    	int len;
	switch (cmd) {
        case IOCTL_SET_DATA:
            if (copy_from_user(arr, (char __user *)arg, sizeof(arr)))
                return -EFAULT;
             len=strlen(arr);
	    int i,j;
	    for( i=0,j=(len-1);i<j;i++,j--){
	    arr[i]^=arr[j]^=arr[i]^=arr[j];
	    }

            break;

        case IOCTL_GET_RESULT:
	    printk(KERN_INFO "the len:%d\n",len);
            if (copy_to_user((char __user *)arg, arr,len+1))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }
    return 0;
}


static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("Device closed\n");
    return 0;
}


static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("Device opened\n");
    return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};


static int __init string_reverse_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register device\n");
        return major;
    }
    pr_info("string rev Loaded (Major: %d)\n", major);
    return 0;
}
static void __exit string_reverse_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("string rev Unloaded\n");
}

module_init(string_reverse_init);
module_exit(string_reverse_exit);



