#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/string.h>

// Use an actual variable instead of #define so we can take its address
static int irq_number = 1;

static char *src_buf = NULL;
static char *dst_buf = NULL;
static int buf_size = 128;

// Work struct
static struct work_struct my_work;

// Custom strrev (not in kernel API)
static void strrev(char *str)
{
    int i, j;
    char temp;
    size_t len = strlen(str);

    for (i = 0, j = len - 1; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

// Workqueue handler (bottom half)
static void work_handler(struct work_struct *work)
{
    printk(KERN_INFO "Workqueue: Started processing\n");

    snprintf(src_buf, buf_size, "original data [%lu]", jiffies);
    memset(dst_buf, 0, buf_size);
    strncpy(dst_buf, src_buf, buf_size - 1);
    strrev(dst_buf);  // Reverse the string

    printk(KERN_INFO "Workqueue: src = %s\n", src_buf);
    printk(KERN_INFO "Workqueue: dst (reversed) = %s\n", dst_buf);
}

// Top half: interrupt handler
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Interrupt received: queuing work\n");
    schedule_work(&my_work);
    return IRQ_HANDLED;
}

// Module init
static int __init my_module_init(void)
{
    int ret;

    printk(KERN_INFO "Workqueue module loading\n");

    src_buf = kmalloc(buf_size, GFP_KERNEL);
    dst_buf = kmalloc(buf_size, GFP_KERNEL);
    if (!src_buf || !dst_buf) {
        printk(KERN_ERR "Memory allocation failed\n");
        return -ENOMEM;
    }

    INIT_WORK(&my_work, work_handler);

    ret = request_irq(irq_number, my_irq_handler, IRQF_SHARED, "my_workqueue_irq", &irq_number);
    if (ret) {
        printk(KERN_ERR "Failed to request IRQ %d\n", irq_number);
        kfree(src_buf);
        kfree(dst_buf);
        return ret;
    }

    printk(KERN_INFO "IRQ %d registered. Press a key to trigger it.\n", irq_number);
    return 0;
}

// Module cleanup
static void __exit my_module_exit(void)
{
    flush_work(&my_work);  // flush only this work, not system-wide
    free_irq(irq_number, &irq_number);
    kfree(src_buf);
    kfree(dst_buf);
    printk(KERN_INFO "Workqueue module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Fixed: Workqueue bottom half from IRQ with custom strrev");

