#include <linux/module.h>
#include <linux/kernel.h>   // Needed for KERN_ALERT
#include <linux/init.h>     // Needed for the macros

MODULE_LICENSE("Dual BSD/GPL"); // ✅ Fixed typo in "LICENSE"

static int __init hello_init(void)   // ✅ Function name and macro
{
    printk(KERN_ALERT "Hello, World\n");  // ✅ Fixed typo: printfk → printk
    return 0;
}

static void __exit hello_exit(void)  // ✅ Should return void
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

