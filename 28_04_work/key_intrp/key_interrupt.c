#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "arithmatic_keys"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static int result;
static int key_count = 1;

struct data {
    int num1, num2, result;
    char op;
};

static struct data *buff_data;

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "%s: Device opened\n",DEVICE_NAME);
    return 0;
}

void perform_operation(int a, int b, unsigned char c) {
    if(c == 0x61 && key_count == 1) {
	pr_info("Addition -> Result: %d\n",a+b);
	key_count++;
    }else if(c == 0x61 && key_count == 2) {
	pr_info("Subtraction -> Result: %d\n",a-b);
	key_count++;
    } else{
	pr_info("INVALID OPERATION\n");
    }
}
	

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "%s: Device closed\n",DEVICE_NAME);
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    int error_count = copy_to_user(buffer, &buff_data->result, sizeof(int));

    return error_count == 0 ? sizeof(struct data) : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {

    if (copy_from_user(&buff_data, buffer, len))
        return -EFAULT;

    message[len] = '\0';
    perform_operation(buff_data->num1, buff_data->num2,buff_data->op);

    /*if (sscanf(message, "%d %c %d", &a, &op, &b) != 3) {
        printk(KERN_ERR "%s: Invalid input format\n",DEVICE_NAME);
        return -EINVAL;
    }

    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        default:
            printk(KERN_ERR "%s: Invalid operator %c\n",DEVICE_NAME, op);
            return -EINVAL;
    }

    printk(KERN_INFO "%s: %d %c %d = %d\n",DEVICE_NAME, a, op, b, result);*/
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init charcalc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "%s failed to register a major number\n",DEVICE_NAME);
        return major;
    }
    printk(KERN_INFO "%s: registered with major number %d\n", DEVICE_NAME, major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "%s: unregistered\n",DEVICE_NAME);
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple character device that performs + and - and using keys");
