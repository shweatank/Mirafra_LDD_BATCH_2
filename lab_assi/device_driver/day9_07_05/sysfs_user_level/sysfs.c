#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple sysfs example");

static struct kobject *my_kobj;
static char my_value[100];

// Sysfs show
static ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
   	size_t len = strlen(my_value);
	memcpy(buf, my_value, len);
	pr_info("sysfs: sent string = %s\n", buf);
	return len;

    
}

// Sysfs store
static ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    size_t len = min(count, sizeof(my_value) - 1);
    strncpy(my_value, buf, len);
    my_value[len] = '\0'; // Null-terminate
    pr_info("sysfs: Received string = %s\n", my_value);
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
