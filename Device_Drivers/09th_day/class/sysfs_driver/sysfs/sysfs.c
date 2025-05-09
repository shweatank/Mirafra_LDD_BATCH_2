#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple sysfs example");

static struct kobject *my_kobj;
static int my_value = 0;

// Sysfs show
static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", my_value);
}

// Sysfs store
static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%d", &my_value);
    return count;
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
