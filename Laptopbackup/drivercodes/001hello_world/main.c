#include<linux/module.h>
#include<linux/init.h>


static int __init helloworld_init(void)
{
    pr_info("hello world \n");
    return 0;
}

static void __exit helloworld_cleanup(void)
{
   pr_info("Good bye world\n");
}


module_init(helloworld_init);
module_exit(helloworld_cleanup);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("GANGA");
MODULE_DESCRIPTION("A SIMPLE HELLO WORLD KERNEL MODULE");
