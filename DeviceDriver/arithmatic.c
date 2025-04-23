#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("Dual BSD/GPL");

int global = 500;

static int hello_init(void)
{
	int a = 10, b = 20;
    printk(KERN_ALERT "Hello, World\n");
    printk(KERN_INFO "Local Addition : %d\n",a+b);
    printk(KERN_INFO "Global Addition : %d\n",z+a+b);
    return 0;
}

static void hello_exit(void)
{
int a = 50, b = 60;
    printk(KERN_ALERT "GoodBye, Cruel World\n");
    printk(KERN_INFO "Local Addition : %d\n",a+b);
    printk(KERN_INFO "Global Addition : %d\n",z+a+b);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohan");
MODULE_DESCRIPTION("A basic Hello World Linux Kernel Module");

