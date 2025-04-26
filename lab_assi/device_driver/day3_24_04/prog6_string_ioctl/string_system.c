#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "string_dri"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, struct calc_data *)
#define SIZE 100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_DESCRIPTION("Kernel Calculator using IOCTL and kmalloc");
MODULE_VERSION("1.0");

struct calc_data {
    char str[SIZE];
};

// Global variables
static int major;
//struct calc_data data1;
static struct calc_data *calc_buffer=NULL;

int count_str(struct calc_data *string){
	int count=0,i=0; 
	while(string->str[i]!='\0'){
		count++;
		i++;
	}
	return count;
}

void string_reversed(struct calc_data *string,int z){
	char temp;
	for(int i=0;i<z/2;i++){
		temp=string->str[i];
		string->str[i]=string->str[z-i-1];
		string->str[z-i-1]=temp;
	}
}
		
// Perform calculation
static void perform_calculation(void) {
	int len=count_str(calc_buffer);
	string_reversed(calc_buffer,len);
	pr_info("reversed string=%s\n",calc_buffer->str);
}

// IOCTL function
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_SET_DATA:
            if (copy_from_user(calc_buffer, (struct calc_data __user *)arg, sizeof(struct calc_data)))
                return -EFAULT;
            perform_calculation();
            break;

        case IOCTL_GET_RESULT:
            if (copy_to_user((struct calc_data __user *)arg,calc_buffer, sizeof(struct calc_data)))
                return -EFAULT;
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

// Device open
static int dev_open(struct inode *inodep, struct file *filep) {
    calc_buffer = kmalloc(sizeof(struct calc_data), GFP_KERNEL);
    if (!calc_buffer) {
        pr_err("Failed to allocate memory\n");
        return -ENOMEM;
    }
    pr_info("Device opened\n");
    return 0;
}

// Device release
static int dev_release(struct inode *inodep, struct file *filep) {
    if (calc_buffer) {
        kfree(calc_buffer);
        pr_info("Memory freed\n");
    }
    pr_info("Device closed\n");
    return 0;
}

// File operations
static struct file_operations fops = {
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

// Module init
static int __init simple_calculator_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register device\n");
        return major;
    }
    pr_info("Sting reversed program Loaded (Major: %d)\n", major);
    return 0;
}

// Module exit
static void __exit simple_calculator_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("String reversed program Unloaded\n");
}

module_init(simple_calculator_init);
module_exit(simple_calculator_exit);

