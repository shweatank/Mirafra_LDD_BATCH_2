#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shubham");
MODULE_DESCRIPTION("Simple sysfs example");

static struct kobject *my_kobj;
static int result = 0;

// Sysfs show
static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", result);
}

// Sysfs store
static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int a, b;
    char op;

    if (sscanf(buf, "%d %c %d", &a, &op, &b) != 3) {
        printk(KERN_ERR "charcalc: Invalid input format\n");
        return -EINVAL;
    }

    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        default:
            printk(KERN_ERR "charcalc: Invalid operator %c\n", op);
            return -EINVAL;
    }

    printk(KERN_INFO "charcalc: %d %c %d = %d\n", a, op, b, result);
    return result;
}

static struct kobj_attribute hello_attribute = __ATTR(hello, 0660, hello_show, hello_store);

static int __init sysfs_example_init(void)
{
    my_kobj = kobject_create_and_add("my_sysfs", kernel_kobj);
    if (!my_kobj)
        return -ENOMEM;

    if (sysfs_create_file(my_kobj, &hello_attribute.attr))
        kobject_put(my_kobj);

    printk(KERN_INFO "sysfs: /sys/kernel/my_sysfs/hello created\n");
    return 0;
}

static void __exit sysfs_example_exit(void)
{
    sysfs_remove_file(my_kobj, &hello_attribute.attr);
    kobject_put(my_kobj);
    printk(KERN_INFO "sysfs: removed\n");
}

module_init(sysfs_example_init);
module_exit(sysfs_example_exit);
