#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/math64.h>
#include <linux/timekeeping.h>
#include <linux/ktime.h>

#define DEVICE_NAME "mychar"
static int major;
static char result_buf[200];

// Get formatted time
void get_time_string(char *buf, size_t buf_size) {
    struct timespec64 ts;
    struct tm tm;
    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm);
    snprintf(buf, buf_size, "[%04ld-%02d-%02d %02d:%02d:%02d] ",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// Reverse a string
void reverse_iteration(char* str) {
    int i = 0;
    int j = strlen(str) - 1;
    for (; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

// File operations - write
ssize_t write_fn(struct file *f, const char __user *ubuf, size_t len, loff_t *off) {
    char buf[200];
    int a, b;
    char time_buf[64];

    if (copy_from_user(buf, ubuf, len)) return -EFAULT;
    buf[len] = '\0';
    sscanf(buf, "%d %d", &a, &b);

    int div = (b != 0) ? (a / b) : 0;
    unsigned long sqrt_b = int_sqrt((unsigned long)b);
    snprintf(result_buf, sizeof(result_buf), "sum=%d diff=%d div=%d mul=%d sqrt=%ld", a + b, a - b, div, a * b, sqrt_b);

    get_time_string(time_buf, sizeof(time_buf));
    printk(KERN_INFO "%s%s\n", time_buf, result_buf);

    return len;
}

// File operations - read
ssize_t read_fn(struct file *f, char __user *ubuf, size_t len, loff_t *off) {
    size_t rlen = strlen(result_buf);
    if (*off >= rlen) return 0;
    if (copy_to_user(ubuf, result_buf, rlen)) return -EFAULT;
    *off += rlen;
    return rlen;
}

// File operations struct
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = write_fn,
    .read = read_fn,
};

// Module Init
static int __init mod_init(void) {
    char a[] = "Akash";
    char time_buf[64];

    major = register_chrdev(0, DEVICE_NAME, &fops);
    get_time_string(time_buf, sizeof(time_buf));
    printk(KERN_INFO "%sDevice registered: /dev/%s with major %d\n", time_buf, DEVICE_NAME, major);

    printk(KERN_INFO "original: %s\n", a);
    reverse_iteration(a);
    printk(KERN_INFO "reverse: %s\n", a);

    return 0;
}

// Module Exit
static void __exit mod_exit(void) {
    char time_buf[64];
    unregister_chrdev(major, DEVICE_NAME);
    get_time_string(time_buf, sizeof(time_buf));
    printk(KERN_INFO "%sDevice unregistered\n", time_buf);
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");

