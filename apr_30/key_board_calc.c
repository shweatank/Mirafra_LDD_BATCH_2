#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>  // for inb()
#define KEYBOARD_IRQ 1  // Keyboard IRQ number
static int count=0;
// Interrupt handler function
#define  DEVICE_NAME "charcal"
#define BUF 100
#define KEYBOARD_DATA_PORT 0x60 

static int major;
static char message[BUF];
static int result;
static int prev,sign=1,a,b;
static unsigned char scancode;
/*static irqreturn_t keyboard_interrupt_handler(int irq,void *dev_id){
unsigned char scancode;
scancode=inb(KEYBOARD_DATA_PORT);
printk(KERN_INFO "key_board interrupt scancode: = 0x%02x\n",scancode);
return IRQ_HANDLER;
}*/
static int dev_open(struct inode *inodep, struct file *filep){
printk(KERN_INFO "charcalc: Device opened\n");
return 0;
}

static int dev_release(struct inode *inodep,struct file *filep){

printk(KERN_INFO "charcal: Device closed");
return 0;
}

static ssize_t dev_read(struct file *filep,char __user *buffer,size_t len,loff_t *offset){
char result_msg[BUF];
int msg_len=snprintf(result_msg,BUF,"Result: %d\n",result);
int error_count=copy_to_user(buffer,result_msg,msg_len);
return error_count == 0 ? msg_len :-EFAULT;
}
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
   // int a, b;
    char op;

    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';
    if (sscanf(message, "%d %d", &a,&b) != 2) {
        printk(KERN_ERR "charcalc: Invalid input format\n");
        return -EINVAL;
    }

    return len;
}
    static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;

    // Read scancode from keyboard data port
    scancode = inb(KEYBOARD_DATA_PORT);

    printk(KERN_INFO "Keyboard interrupt: scancode = 0x%02x\n", scancode);
	switch(scancode){
		case 0x0e:result=a+b;
			  break;
		default : result=a*b;
			  break;
	}
    return IRQ_HANDLED;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init my_init(void)
{
    int result;
     major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "charcalc failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "charcalc: registered with major number %d\n", major);


    printk(KERN_INFO "Keyboard Interrupt Module Loaded\n");

    // Request IRQ for the keyboard
    result = request_irq(KEYBOARD_IRQ,            // IRQ number
                         keyboard_interrupt_handler, // Interrupt handler
                         IRQF_SHARED,                // Shared IRQ flag
                         "keyboard_interrupt",       // Name
                         (void *)(keyboard_interrupt_handler)); // Device ID for sharing

    if (result) {
        printk(KERN_ERR "Failed to register keyboard IRQ\n");
        return result;
    }

    return 0;
}

static void __exit my_exit(void)
{
    // Free the IRQ
     unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "charcalc: unregistered\n");

    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard Interrupt Module Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple Linux Keyboard Interrupt Handler Example");
