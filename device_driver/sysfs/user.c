#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Sysfs Full Calculator");
MODULE_VERSION("1.0");

static struct kobject *calc_kobj;
static int num1 = 0, num2 = 0, result = 0;
static char operation = '+';

static ssize_t num1_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%d", &num1);
    return count;
}

static ssize_t num2_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    sscanf(buf, "%d", &num2);
    return count;
}

static ssize_t operation_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    if (buf[0] == '+' || buf[0] == '-' || buf[0] == '*' || buf[0] == '/')
        operation = buf[0];
    return count;
}

static ssize_t result_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    switch (operation) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/': result = (num2 != 0) ? (num1 / num2) : 0; break;
        default: result = 0; break;
    }
    return sprintf(buf, "%d\n", result);
}

static struct kobj_attribute num1_attr = __ATTR(num1, 0660, NULL, num1_store);
static struct kobj_attribute num2_attr = __ATTR(num2, 0660, NULL, num2_store);
static struct kobj_attribute op_attr   = __ATTR(operation, 0660, NULL, operation_store);
static struct kobj_attribute res_attr  = __ATTR(result, 0444, result_show, NULL);

static int __init calc_init(void) {
    calc_kobj = kobject_create_and_add("calc_sysfs", kernel_kobj);
    if (!calc_kobj)
        return -ENOMEM;

    sysfs_create_file(calc_kobj, &num1_attr.attr);
    sysfs_create_file(calc_kobj, &num2_attr.attr);
    sysfs_create_file(calc_kobj, &op_attr.attr);
    sysfs_create_file(calc_kobj, &res_attr.attr);

    printk(KERN_INFO "Sysfs Calculator Loaded.\n");
    return 0;
}

static void __exit calc_exit(void) {
    sysfs_remove_file(calc_kobj, &num1_attr.attr);
    sysfs_remove_file(calc_kobj, &num2_attr.attr);
    sysfs_remove_file(calc_kobj, &op_attr.attr);
    sysfs_remove_file(calc_kobj, &res_attr.attr);
    kobject_put(calc_kobj);
    printk(KERN_INFO "Sysfs Calculator Unloaded.\n");
}

module_init(calc_init);
module_exit(calc_exit);

