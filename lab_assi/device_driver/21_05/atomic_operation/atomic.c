#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/atomic.h>

static atomic_t my_counter = ATOMIC_INIT(0);

static int __init atomic_example_init(void) {
    printk(KERN_INFO "Atomic Counter Init: %d\n", atomic_read(&my_counter));
    atomic_inc(&my_counter);
    printk(KERN_INFO "After Increment: %d\n", atomic_read(&my_counter));
    return 0;
}

static void __exit atomic_example_exit(void) {
    atomic_dec(&my_counter);
    printk(KERN_INFO "Atomic Counter Exit: %d\n", atomic_read(&my_counter));
}

module_init(atomic_example_init);
module_exit(atomic_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("Simple atomic_t example");

