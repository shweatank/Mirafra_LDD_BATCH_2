// main.c
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define DEVICE_NAME "keyboard_device"
#define CLASS_NAME  "keyboard_class"
#define KEYBOARD_IRQ 1    // PS/2 keyboard IRQ

static int    major_number;
static struct class*  my_class  = NULL;
static struct device* my_device = NULL;

static int    a = 0, b = 0;
static unsigned int click_count  = 0;
static unsigned long last_jiffies = 0;

// IRQ handler: debounced, cycles through +, –, ×, ÷
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned long now = jiffies;

    // debounce: ignore if <50 ms since last
    if (time_before(now, last_jiffies + msecs_to_jiffies(50)))
        return IRQ_HANDLED;
    last_jiffies = now;

    click_count++;
    switch ((click_count - 1) % 4) {
    case 0:
        printk(KERN_INFO "[%s] %d + %d = %d\n",
               DEVICE_NAME, a, b, a + b);
        break;
    case 1:
        printk(KERN_INFO "[%s] %d - %d = %d\n",
               DEVICE_NAME, a, b, a - b);
        break;
    case 2:
        printk(KERN_INFO "[%s] %d * %d = %d\n",
               DEVICE_NAME, a, b, a * b);
        break;
    case 3:
        if (b != 0)
            printk(KERN_INFO "[%s] %d / %d = %d\n",
                   DEVICE_NAME, a, b, a / b);
        else
            printk(KERN_WARNING "[%s] Division by zero!\n",
                   DEVICE_NAME);
        break;
    }
    return IRQ_HANDLED;
}

// write(): parse “<a> <b>”
static ssize_t dev_write(struct file *filep, const char __user *buf,
                         size_t len, loff_t *off)
{
    char kbuf[64];
    int ra, rb;

    if (len >= sizeof(kbuf)) len = sizeof(kbuf)-1;
    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;
    kbuf[len] = '\0';

    if (sscanf(kbuf, "%d %d", &ra, &rb) == 2) {
        a = ra;  b = rb;
        printk(KERN_INFO "[%s] Set a=%d, b=%d\n", DEVICE_NAME, a, b);
    } else {
        printk(KERN_WARNING "[%s] write must be: <int> <int>\n",
               DEVICE_NAME);
    }
    return len;
}

static int     dev_open(struct inode *inode, struct file *file)   { return 0; }
static int     dev_release(struct inode *inode, struct file *file){ return 0; }

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .write   = dev_write,
};

static int __init kmod_init(void)
{
    int ret;
    printk(KERN_INFO "[%s] init\n", DEVICE_NAME);

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "[%s] reg_chrdev failed\n", DEVICE_NAME);
        return major_number;
    }

    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "[%s] class_create failed\n", DEVICE_NAME);
        return PTR_ERR(my_class);
    }

    my_device = device_create(my_class, NULL,
                              MKDEV(major_number, 0),
                              NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "[%s] device_create failed\n", DEVICE_NAME);
        return PTR_ERR(my_device);
    }

    ret = request_irq(KEYBOARD_IRQ,
                      keyboard_interrupt_handler,
                      IRQF_SHARED,
                      "keyboard_interrupt_ops",
                      (void*)keyboard_interrupt_handler);
    if (ret) {
        device_destroy(my_class, MKDEV(major_number, 0));
        class_destroy(my_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ERR "[%s] request_irq failed\n", DEVICE_NAME);
        return ret;
    }

    printk(KERN_INFO "[%s] ready: /dev/%s (major=%d)\n",
           DEVICE_NAME, DEVICE_NAME, major_number);
    return 0;
}

static void __exit kmod_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void*)keyboard_interrupt_handler);
    device_destroy(my_class, MKDEV(major_number, 0));
    class_destroy(my_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "[%s] exit\n", DEVICE_NAME);
}

module_init(kmod_init);
module_exit(kmod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard calc: write two ints, then keypress cycles + - * /");
