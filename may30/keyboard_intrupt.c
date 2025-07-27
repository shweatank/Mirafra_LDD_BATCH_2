#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "kbdchar"
#define IRQ_NUM 1  // IRQ1 is for PS/2 keyboard

static int major;
static struct cdev kbd_cdev;

static char last_scancode = 0;

// Interrupt handler
static irqreturn_t kbd_interrupt_handler(int irq, void *dev_id) {
    last_scancode = inb(0x60);  // Read from keyboard data port
    printk(KERN_INFO "kbdchar: Interrupt! Scancode = 0x%x\n", last_scancode);
    return IRQ_HANDLED;
}

// File operations
static ssize_t kbd_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (copy_to_user(buf, &last_scancode, 1))
        return -EFAULT;
    return 1;
}

static int kbd_open(struct inode *inode, struct file *file) {
    return 0;
}

static int kbd_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = kbd_open,
    .read = kbd_read,
    .release = kbd_release,
};

static int __init kbd_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "kbdchar: Failed \n");
        return major;
    }

    // Request IRQ 1 (keyboard IRQ)
    if (request_irq(IRQ_NUM, kbd_interrupt_handler, IRQF_SHARED, DEVICE_NAME, (void *)(kbd_interrupt_handler))) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ERR "kbdcharCannot register IRQ %d\n", IRQ_NUM);
        return -EIO;
    }

    printk(KERN_INFO "kbdchar: Module loaded. Major = %d\n", major);
    return 0;
}

static void __exit kbd_exit(void) {
    free_irq(IRQ_NUM, (void *)(kbd_interrupt_handler));
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "kbdcharModule unloaded\n");
}

module_init(kbd_init);
module_exit(kbd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEJA");
MODULE_DESCRIPTION("Keyboard IRQ Handler Character Driver");
