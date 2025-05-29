#include <linux/module.h>
#include <linux/kernel.h>

static int irq = 5;
static char *device_name = "my_device";

module_param(irq, int, 0444);             // int parameter with read-only permissions
module_param(device_name, charp, 0444);   // string (char pointer) parameter

MODULE_PARM_DESC(irq, "IRQ number for the device");
MODULE_PARM_DESC(device_name, "Name of the device");

static int __init param_demo_init(void) {
    printk(KERN_INFO "param_demo: irq = %d\n", irq);
    printk(KERN_INFO "param_demo: device_name = %s\n", device_name);
    return 0;
}

static void __exit param_demo_exit(void) {
    printk(KERN_INFO "param_demo: Module unloaded\n");
}

module_init(param_demo_init);
module_exit(param_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Example of parameter passing to kernel module");

