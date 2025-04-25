#include <linux/module.h>
#include <linux/kernel.h>   // Needed for KERN_ALERT
#include <linux/init.h>     // Needed for the macros

MODULE_LICENSE("Dual BSD/GPL"); // ✅ Fixed typo in "LICENSE"
int a=20,b=10;
static int __init hello_init(void)   // ✅ Function name and macro
{
    int ret=a+b;
    printk(KERN_ALERT "add=%d\n",ret);  // ✅ Fixed typo: printfk → printk
    return 0;
}

static void __exit hello_exit(void)  // ✅ Should return void
{
int ret=a-b;
    printk(KERN_ALERT "sub=%d\n",ret);
}

module_init(hello_init);
module_exit(hello_exit);

