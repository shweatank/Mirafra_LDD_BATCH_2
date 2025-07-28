#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Demo");
MODULE_DESCRIPTION("Kernel Divide by Zero Example");

static int __init divzero_init(void) {
    int a = 1;
    int b = 0;
    int c;

    printk(KERN_INFO "Loading divide by zero module...\n");

    c = a / b;  // 🚨 This will trigger a divide-by-zero exception

    printk(KERN_INFO "Result: %d\n", c);
    return 0;
}

static void __exit divzero_exit(void) {
    printk(KERN_INFO "Exiting module.\n");
}

module_init(divzero_init);
module_exit(divzero_exit);

