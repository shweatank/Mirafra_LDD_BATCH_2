
 #include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/math64.h>

#define DEVICE_NAME "charcale"
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
    int msg_len = snprintf(result_msg, BUF_LEN, "Result: %d, Square Root: %lf\n", result, sqrt(result));
    int error_count = copy_to_user(buffer, result_msg, msg_len);

    return error_count == 0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    if (copy_from_user(message, buffer, len)) {
        return -EFAULT;
    }

    message[len] = '\0';  // Null terminate the string
    reverse_string(message);  // Reverse the received input
    
    kstrtoint(message, 10, &result); // Convert string input to integer if possible
    printk(KERN_INFO "charcalc: Received and reversed string %s\n", message);

    return len;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    if (copy_from_user(message, buffer, len)) {
        return -EFAULT;
    }

    message[len] = '\0';  // Null terminate the string
    kstrtoint(message, 10, &result); // Convert string input to integer
    printk(KERN_INFO "charcalc: Received number %d\n", result);

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
        printk(KERN_ALERT "charcalc: Failed to register character device\n");
        return major;
    }

    printk(KERN_INFO "charcalc: Registered with major number %d\n", major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "charcalc: Unregistered device\n");
}


