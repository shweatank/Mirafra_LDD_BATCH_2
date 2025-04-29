#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/io.h>  // for inb()

#define KEYBOARD_IRQ 1     // Standard keyboard IRQ
#define KEYBOARD_DATA_PORT 0x60  // Data port for keyboard

#define DEVICE_NAME "my_key_intr"
//static int major;

struct data {
    int num1, num2, result;
    char op;
};

struct data *buff_data;

int key_count = 1;

void perform_operation(int a, int b, unsigned char c) {
    if(c == 0x61 && key_count == 1) {
	pr_info("Addition -> Result: %d\n",a+b);
	key_count++;
    }else if(c == 0x61 && key_count == 2) {
	pr_info("Subtraction -> Result: %d\n",a-b);
	key_count++;
    } else{
	pr_info("INVALID OPERATION\n");
    }
}
	

// Interrupt handler function
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    // Read scancode from keyboard data port
    scancode = inb(KEYBOARD_DATA_PORT);

    printk(KERN_INFO "Keyboard interrupt: scancode = 0x%02x\n", scancode);
    perform_operation(num1,num2,scancode);

    return IRQ_HANDLED;

}

// File operations - write
ssize_t write_fn(struct file *f, const char __user *ubuf, size_t len, loff_t *off) {

    if (copy_from_user(buff_data, (struct data *)arg, sizeof(struct data))) return -EFAULT;

    perform_operation(buff_data->num1, buff_data->num2,buff_data->op);

    return len;
}

ssize_t read_fn(struct file *f, char __user *ubuf, size_t len, loff_t *off) {
    size_t rlen = strlen(result_buf);
    if (*off >= rlen) return 0;
    if (copy_to_user(buff_data->result, r, rlen)) return -EFAULT;
    *off += rlen;
    return rlen;
}

// File operations struct
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = write_fn,
    .read = read_fn,
};

static int __init my_init(void)
{
    int result;
    //major = register_chrdev(0, DEVICE_NAME, &fops);
    
    printk(KERN_INFO "Keyboard Interrupt Module with Scancode Reading Loaded\n");
    
    printk(KERN_INFO "Device registered: /dev/%s\n", DEVICE_NAME);

    // Request IRQ for the keyboard
    result = request_irq(KEYBOARD_IRQ,
                         keyboard_interrupt_handler,
                         IRQF_SHARED,
                         "keyboard_interrupt_scancode",
                         (void *)(keyboard_interrupt_handler));

    if (result) {
        printk(KERN_ERR "Failed to register keyboard IRQ\n");
        return result;
    }

    return 0;
}

static void __exit my_exit(void)
{
    //unregister_chrdev(major, DEVICE_NAME);
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard Interrupt Module with Scancode Reading Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard Interrupt Handler Reading Scancode Example");

/* 
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
    if (sscanf(message, "%d %c %d", &a, &op, &b) != 3) {
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
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init charcalc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "charcalc failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "charcalc: registered with major number %d\n", major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "charcalc: unregistered\n");
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple character device that performs + and -");
   *\
