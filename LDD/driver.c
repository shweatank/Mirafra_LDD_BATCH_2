#include<linux/init.h>
#include<linux/module.h>
#include<linux/device.h>


static int __init mychar_init(void){
int a=10;
int b=0;
int x=a/b;
printk(KERN_INFO "the init function");
printk(KERN_INFO "the zero denominator:%d",x);
return 0;
}
static void __exit mychar_exit(void){
printk(KERN_INFO "the exit function");
}
module_init(mychar_init);
module_exit(mychar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Teja");
MODULE_DESCRIPTION("Basic Character Device Driver");
