#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>



static int __init hello_init(void)
{
	set_log_level(LOG_WARNING);  // Set the minimum log level to debug

    // Example log messages
    log_message(LOG_DEBUG, "This is a debug message");
    log_message(LOG_INFO, "This is an info message");
    log_message(LOG_WARNING, "This is a warning message");
    log_message(LOG_ERROR, "This is an error message");

    return 0;
}
static void __exit hello_exit(void)
{
	printk(KERN_INFO "ok");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
