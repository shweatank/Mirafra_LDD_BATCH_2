#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_NAME "my_proc"
#define BUF_LEN 100

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple procfs example");

static char proc_buf[BUF_LEN];
static int len = 0;

// read from /proc/my_proc
static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    return simple_read_from_buffer(buf, count, ppos, proc_buf, len);
}

// write to /proc/my_proc
static ssize_t proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    len = min(count, (size_t)(BUF_LEN - 1));
    if (copy_from_user(proc_buf, buf, len))
        return -EFAULT;
    proc_buf[len] = '\0';
    return count;
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
