/*Bottom Half Using Tasklets

Implement a tasklet that processes some dummy data (e.g., increment a counter) whenever an interrupt occurs.

Log messages using printk to show tasklet execution.*/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>  // For tasklets
#include <linux/timer.h>      // For simulation
void timer_handler(struct timer_list *t);
void tasklet_function(unsigned long data);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("Tasklet Example - Bottom Half");

static struct tasklet_struct my_tasklet;
static struct timer_list my_timer;
static int counter = 0;

void tasklet_function(unsigned long data) {
    counter++;
    printk(KERN_INFO "Tasklet Executed: Counter = %d\n", counter);
}

// Simulated interrupt trigger using timer
void timer_handler(struct timer_list *t) {
    printk(KERN_INFO "Simulated interrupt: Scheduling tasklet\n");
    tasklet_schedule(&my_tasklet);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));  // Repeat every 1 sec
}

static int __init my_init(void) {
    printk(KERN_INFO "Tasklet Module Init\n");

    tasklet_init(&my_tasklet, tasklet_function, 0);

    // Start simulated interrupt using timer
    timer_setup(&my_timer, timer_handler, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    return 0;
}

static void __exit my_exit(void) {
    printk(KERN_INFO "Cleaning up Tasklet Module\n");

    tasklet_kill(&my_tasklet);
    del_timer(&my_timer);
}

module_init(my_init);
module_exit(my_exit);
