#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_NAME "my_proc"
#define BUF_LEN 100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_DESCRIPTION("Simple procfs example");

static char proc_buf[BUF_LEN];
static char result_str[BUF_LEN];
static int result = 0;
static int len;

// read from /proc/my_proc
static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    len = snprintf(result_str, BUF_LEN, "%d\n", result);
    return simple_read_from_buffer(buf, count, ppos, result_str, len);
}

// write to /proc/my_proc
static ssize_t proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int a, b;
    char op;

    len = min(count, (size_t)(BUF_LEN - 1));
    if (copy_from_user(proc_buf, buf, len))
        return -EFAULT;
    proc_buf[len] = '\0';

    if (sscanf(proc_buf, "%d %c %d", &a, &op, &b) != 3) {
        printk(KERN_ERR "charcalc: Invalid input format\n");
        return -EINVAL;
    }

    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        default:
            printk(KERN_ERR "charcalc: Invalid operator %c\n", op);
            return -EINVAL;
    }

    printk(KERN_INFO "charcalc: %d %c %d = %d\n", a, op, b, result);
    return count;  // Correct return value
}

static const struct proc_ops proc_file_ops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init procfs_example_init(void)
{
    proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);
    printk(KERN_INFO "procfs: /proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit procfs_example_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "procfs: /proc/%s removed\n", PROC_NAME);
}

module_init(procfs_example_init);
module_exit(procfs_example_exit);

