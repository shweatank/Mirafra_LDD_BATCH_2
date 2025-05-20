#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>  // for inb()
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME       "kernel"
#define KEYBOARD_IRQ 1     // Standard keyboard IRQ
#define KEYBOARD_DATA_PORT 0x60  // Data port for keyboard //standard data port for keyboard

static bool enable_irq_handler = false;

static char message[100];

static int dev_open(struct inode *inodep, struct file *filep) {
      printk(KERN_INFO "charcalc: Device opened\n");
      return 0;
 }
  
  static int dev_release(struct inode *inodep, struct file *filep) {
      printk(KERN_INFO "charcalc: Device closed\n");
      return 0;
 }


// Interrupt handler function
static irqreturn_t keyboard_interrupt_handler(int irq, void *dev_id)
{
    unsigned char scancode;
	if (!enable_irq_handler)
        return IRQ_NONE;

    // Read scancode from keyboard data port
    scancode = inb(KEYBOARD_DATA_PORT);       //inb(),used to read a byte from I/O Port

    printk(KERN_INFO "Keyboard interrupt: scancode = 0x%02x\n", scancode);
	printk(KERN_INFO "addition: %d\n",3+4);

    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    int result;

    printk(KERN_INFO "Keyboard Interrupt Module with Scancode Reading Loaded\n");

    // Request IRQ for the keyboard
	//if(message[0]=='a')
	//{
    result = request_irq(KEYBOARD_IRQ,
                         keyboard_interrupt_handler,
                         IRQF_SHARED,
                         "keyboard_interrupt_scancode",
                         (void *)(keyboard_interrupt_handler));
	//}

    if (result) {
        printk(KERN_ERR "Failed to register keyboard IRQ\n");
        return result;
    }

    return 0;
}

static void __exit my_exit(void)
{
    free_irq(KEYBOARD_IRQ, (void *)(keyboard_interrupt_handler));
    printk(KERN_INFO "Keyboard Interrupt Module with Scancode Reading Unloaded\n");
}
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
  
      if (copy_from_user(message, buffer, len))
          return -EFAULT;

	  message[len]='\0';
	  printk(KERN_INFO, "Received data: %s\n",message);
	  if (message[0] == 'a') {
        enable_irq_handler = true;
        printk(KERN_INFO "Keyboard IRQ handler enabled\n");
    } else {
        enable_irq_handler = false;
        printk(KERN_INFO "Keyboard IRQ handler disabled\n");
    }
      return len;
  }

static struct file_operations fops = {
      .open = dev_open,
      .write = dev_write,
      .release = dev_release,
  };
  
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Keyboard Interrupt Handler Reading Scancode Example");
