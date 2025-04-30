#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/keyboard.h>
#include<linux/io.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define KEYBOARD_IRQ 1  // Keyboard IRQ number
#define DEVICE_NAME "keyboard_data"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static int a = 0, b = 0;   // Operands
static int result = 0;

static int dev_id;
static struct timer_list my_timer;

static void my_timer_callback(struct timer_list *t)
{
    pr_info("Timer fired! jiffies = %lu\n", jiffies);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}

static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    scancode = inb(0x60); // Read scancode from keyboard controller
        if (scancode & 0x80)
   	 return IRQ_HANDLED;

    printk(KERN_INFO "Keyboard interrupt received! Scancode: 0x%02x\n", scancode);

    if (scancode == 0x1e) { // 'a' key press (scancode 0x1e)
        result = a + b;
        printk(KERN_INFO "Operation: %d + %d = %d\n", a, b, result);
    } else if (scancode == 0x30) { // 'b' key press (scancode 0x30)
        result = a - b;
        printk(KERN_INFO "Operation: %d - %d = %d\n", a, b, result);
    }
    else if (scancode == 0x2e) { // 'b' key press (scancode 0x30)
        result = a * b;
        printk(KERN_INFO "Operation: %d * %d = %d\n", a, b, result);
        }
    else{
        result = 0;
       // printk(KERN_INFO "Invalid key pressed. Result set to 0.\n");
    }

    return IRQ_HANDLED;
}

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
   
    int msg_len = snprintf(result_msg, BUF_LEN, "%d\n", result);
    int error_count = copy_to_user(buffer, result_msg, msg_len);

    return error_count == 0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';
    if (sscanf(message, "%d %d", &a, &b) != 2) {
        printk(KERN_ERR "charcalc: Invalid input format. Expected: a b\n");
        return -EINVAL;
    }
   
    printk(KERN_INFO "charcalc: Stored numbers: %d and %d\n", a, b);
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init my_init(void)
{
    int ret;

    printk(KERN_INFO "Keyboard Interrupt Module Loaded\n");

    ret = request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler, IRQF_SHARED, "keyboard_interrupt", &dev_id);
    if (ret) {
        printk(KERN_ERR "Failed to register keyboard IRQ\n");
        return ret;
    }

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "charcalc failed to register a major number\n");
        free_irq(KEYBOARD_IRQ, &dev_id);
        return major;
    }

    printk(KERN_INFO "charcalc: registered with major number %d\n", major);
    
     // Timer Setup
    timer_setup(&my_timer, my_timer_callback, 0);

    // Set timer to fire 2 seconds from now
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    pr_info("Timer module loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    del_timer_sync(&my_timer);
    free_irq(KEYBOARD_IRQ, &dev_id);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Keyboard Interrupt Module Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_DESCRIPTION("Simple Linux Keyboard Interrupt Calculator Example");

