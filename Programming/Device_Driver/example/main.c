#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
int a = 10;
int b = 5;
static int hello_init(void)
{
    printk(KERN_ALERT "Hello, World\n");
    int sum = a + b;
    printk(KERN_ALERT "sum of a and b: %d\n", sum);
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "GoodBye, Cruel World\n");
    int diff = a - b;
    printk(KERN_ALERT "diff of a and b: %d\n", diff);
    
}

module_init(hello_init);
module_exit(hello_exit);
