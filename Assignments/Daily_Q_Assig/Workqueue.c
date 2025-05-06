//Bottom Half Using Workqueue


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/string.h>

#define SHARED_IRQ 1  // IRQ 1 is keyboard (for test/demo purposes)

static int irq = SHARED_IRQ;

// Dummy data
static char src_buffer[100] = "Hello from Top Half!";
static char dest_buffer[100];

// Work structure
static struct work_struct my_work;

// Workqueue handler
static void workqueue_handler(struct work_struct *work)
{
    printk(KERN_INFO "Workqueue: Copying and modifying string...\n");

    // Copy data from src_buffer to dest_buffer
    strncpy(dest_buffer, src_buffer, sizeof(dest_buffer));

    // Dummy manipulation: make all chars uppercase
    for (int i = 0; dest_buffer[i] != '\0'; i++) {
        if (dest_buffer[i] >= 'a' && dest_buffer[i] <= 'z') {
            dest_buffer[i] -= 32;
        }
    }

    printk(KERN_INFO "Workqueue: Modified string = %s\n", dest_buffer);
}

// IRQ Handler (Top Half)
static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
    printk(KERN_INFO "Top Half: Interrupt received, scheduling workqueue\n");

    schedule_work(&my_work);

    return IRQ_HANDLED;
}

// Module Init
static int __init workqueue_module_init(void)
{
    int ret;

    printk(KERN_INFO "Workqueue Module loading...\n");

    // Initialize work
    INIT_WORK(&my_work, workqueue_handler);

    // Request IRQ
    ret = request_irq(irq, my_irq_handler, IRQF_SHARED, "my_workqueue_irq", &irq);
    if (ret) {
        printk(KERN_ERR "Failed to register IRQ %d\n", irq);
        return ret;
    }

    printk(KERN_INFO "IRQ %d registered successfully\n", irq);
    return 0;
}

// Module Exit
static void __exit workqueue_module_exit(void)
{
    flush_scheduled_work();  // Wait for pending work to finish
    free_irq(irq, &irq);
    printk(KERN_INFO "Workqueue module unloaded. IRQ %d freed.\n", irq);
}

module_init(workqueue_module_init);
module_exit(workqueue_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohan");
MODULE_DESCRIPTION("Example of Bottom Half using Workqueue triggered from ISR");
