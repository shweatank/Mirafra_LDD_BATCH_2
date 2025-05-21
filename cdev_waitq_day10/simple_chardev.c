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
    int msg_len = snprintf(result_msg, BUF_LEN, "Result: %d\n", result);
    int error_count = copy_to_user(buffer, result_msg, msg_len);

    return error_count == 0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    int a, b;
    char op;

    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';

	pr_info("Write called: waking up reader...\n");
    if (sscanf(message, "%d %c %d", &a, &op, &b) != 3) {
        printk(KERN_ERR "charcalc: Invalid input format\n");
        return -EINVAL;
    }

    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': result = a / b; break;
        default:
            printk(KERN_ERR "charcalc: Invalid operator %c\n", op);
            return -EINVAL;
    }

    printk(KERN_INFO "charcalc: %d %c %d = %d\n", a, op, b, result);
     // Set flag and wake up wait queue
    data_ready = 1;
    wake_up_interruptible(&my_wq);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};


static int __init my_init(void)
{
    int ret;

    // Allocate major and minor number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    // Init cdev
    cdev_init(&my_cdev, &fops);
    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0)
        goto unregister;

    // ✅ Create class (correct API for 6.8 kernel)
    my_class = class_create(THIS_MODULE,"my_waitq_class");
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        goto del_cdev;
    }

    // Create device node in /dev
    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        ret = PTR_ERR(my_device);
        goto destroy_class;
    }

    // Initialize wait queue
    init_waitqueue_head(&my_wq);

    pr_info("Wait queue driver loaded.\n");
    return 0;

destroy_class:
    class_destroy(my_class);
del_cdev:
    cdev_del(&my_cdev);
unregister:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("Wait queue driver unloaded.\n");
}

module_init(my_init);
module_exit(my_exit);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("venkat");
MODULE_DESCRIPTION("A simple character device that performs + and -");
