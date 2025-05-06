//Testing & Verification


// kernel
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#define DEVICE_NAME "irq_sim"
#define BUF_LEN 128

static int major;
static char kernel_buf[BUF_LEN] = "Initial Data\n";
static struct work_struct my_work;

/* Workqueue function (bottom half) */
static void workqueue_handler(struct work_struct *work)
{
    printk(KERN_INFO "Workqueue: Running bottom half\n");
    snprintf(kernel_buf, BUF_LEN, "Processed by workqueue\n");
}

/* Simulate interrupt (top half) */
static void simulate_irq(void)
{
    printk(KERN_INFO "Top half: Simulated interrupt triggered\n");
    schedule_work(&my_work);
}

/* File ops */
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    simulate_irq();  // simulate IRQ on write
    return len;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    size_t data_len = strlen(kernel_buf);
    if (len < data_len)
        return -EINVAL;

    if (copy_to_user(buf, kernel_buf, data_len))
        return -EFAULT;

    return data_len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
};

static int __init irqsim_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "Failed to register char device\n");
        return major;
    }

    INIT_WORK(&my_work, workqueue_handler);

    printk(KERN_INFO "Module loaded. Use: mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
    return 0;
}

static void __exit irqsim_exit(void)
{
    flush_scheduled_work();
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Module unloaded\n");
}

module_init(irqsim_init);
module_exit(irqsim_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Simulated IRQ with workqueue (no cdev)");

// user program
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
    char buffer[128] = {0};

    int fd = open("/dev/irq_sim", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    write(fd, "trigger", 7);
    sleep(1); // allow workqueue to run
    read(fd, buffer, sizeof(buffer));

    printf("Read from device: %s", buffer);
    close(fd);
    return 0;
}
