#include <Linux/init.h>
#include <Linux/module.h>

MODULE LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
	printk(KERN_ALERT, "Hello world \n");
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT, "Goodbye, Cruel world \n");
}

module_init(hello_init);
module_exit(hello_exit);
