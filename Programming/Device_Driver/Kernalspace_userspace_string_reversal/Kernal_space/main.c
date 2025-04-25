#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "mychar"
static int major;
static char result_buf[200];

void reverse_iteration(char* str)
{
    int i = 0;
    int j = strlen(str) - 1;

    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

ssize_t write_fn(struct file *f, const char __user *ubuf, size_t len, loff_t *off) {
    char buf[200];

    if (len > sizeof(buf) - 1)
        return -EINVAL;

    if (copy_from_user(buf, ubuf, len)) return -EFAULT;
    buf[len] = '\0';

    reverse_iteration(buf);
    snprintf(result_buf, sizeof(result_buf), "%s", buf);
    return len;
}

ssize_t read_fn(struct file *f, char __user *ubuf, size_t len, loff_t *off) {
    size_t rlen = strlen(result_buf);
    if (*off >= rlen) return 0;
    if (copy_to_user(ubuf, result_buf, rlen)) return -EFAULT;
    *off += rlen;
    return rlen;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = write_fn,
    .read = read_fn,
};

static int __init mod_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "Device registered: /dev/%s with major %d\n", DEVICE_NAME, major);
    return 0;
}

static void __exit mod_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");

