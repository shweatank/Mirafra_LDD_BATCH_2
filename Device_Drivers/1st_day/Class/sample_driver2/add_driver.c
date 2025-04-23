#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A Simple Hello World Linux Kernel Module");
int sum=0;
int diff =0;
static int __init hello_init(void) {
	int a=30, b= 10;
	sum = a+b;
	diff = a-b;
    printk(KERN_INFO "Hello, Kernel!\n");
    return 0;
}

static void __exit hello_exit(void) {

    printk(KERN_INFO "Goodbye, Kernel! \nThe Sum %d\nThe difference %d\n", sum, diff);
}

module_init(hello_init);
module_exit(hello_exit);
