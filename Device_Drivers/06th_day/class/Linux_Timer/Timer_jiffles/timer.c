#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define DEVICE_NAME "timerdev"

static struct timer_list my_timer;
static int major;

static void my_timer_callback(struct timer_list *t)
{
    pr_info("Timer fired! jiffies = %lu\n", jiffies);
}

static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
};

static int __init timer_module_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register device\n");
        return major;
    }

    pr_info("TimerDev registered: major number %d\n", major);

    // Timer Setup
    timer_setup(&my_timer, my_timer_callback, 0);

    // Set timer to fire 2 seconds from now
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    pr_info("Timer module loaded\n");

    return 0;
}

static void __exit timer_module_exit(void)
{
    del_timer_sync(&my_timer);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Timer module unloaded\n");
}

module_init(timer_module_init);
module_exit(timer_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Linux Timer Device Driver Example");
