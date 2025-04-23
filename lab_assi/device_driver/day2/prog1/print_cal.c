#include<linux/init.h>
#include<linux/module.h>

MODULE_LICENSE("DUAL BSD/GPL");
int z=100;
static int hello_init(void){
        int a=10,b=20;
	printk("in hello world\n");
	printk("local addition=%d\n",a+b);
	printk("gobal addition=%d\n",z+a+b);
        return 0;
}
static void hello_exit(void){
	int c=20,d=10;
        printk(KERN_ALERT "Good bye, cruel world\n");
	printk("local substraction=%d\n",c-d);
	printk("gobal substraction=%d\n",z-c-d);
}

module_init(hello_init);
module_exit(hello_exit);
