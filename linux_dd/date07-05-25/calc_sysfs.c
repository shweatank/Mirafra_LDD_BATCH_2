#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>    // kobject, kobj_type
#include <linux/sysfs.h>      // sysfs interface
#include <linux/string.h>     // sscanf
#include <linux/slab.h>       // kmalloc, kfree

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple calculator using sysfs");
MODULE_VERSION("0.1");

static struct kobject *calculator_kobj;
static int result;
static char last_input[256] = {0};

// Perform the calculation based on input
static void calculate(const char *buf) {
    int a, b;
    char op;

    if (sscanf(buf, "%d %c %d", &a, &op, &b) == 3) {
        switch (op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/': result = (b != 0) ? (a / b) : -1; break;
            default:
                printk(KERN_WARNING "Unknown operation: %c\n", op);
                result = -1;
                return;
        }
        snprintf(last_input, sizeof(last_input), "%d %c %d = %d", a, op, b, result);
        printk(KERN_INFO "Calculated: %s\n", last_input);
    } else {
        printk(KERN_WARNING "Invalid input format\n");
        snprintf(last_input, sizeof(last_input), "Invalid input");
        result = -1;
    }
}

// sysfs write handler
static ssize_t input_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t count) {
    calculate(buf);
    return count;
}

// sysfs read handler for result
static ssize_t result_show(struct kobject *kobj, struct kobj_attribute *attr,
                           char *buf) {
    return sprintf(buf, "%d\n", result);
}

// Optional: read the last full expression
static ssize_t input_show(struct kobject *kobj, struct kobj_attribute *attr,
                          char *buf) {
    return sprintf(buf, "%s\n", last_input);
}

// Define sysfs attributes
static struct kobj_attribute input_attribute =
    __ATTR(input, 0660, input_show, input_store);

static struct kobj_attribute result_attribute =
    __ATTR(result, 0440, result_show, NULL);

// Attribute group
static struct attribute *attrs[] = {
    &input_attribute.attr,
    &result_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

// Init and Exit
static int __init calculator_sysfs_init(void) {
    calculator_kobj = kobject_create_and_add("calculator", kernel_kobj);
    if (!calculator_kobj)
        return -ENOMEM;

    if (sysfs_create_group(calculator_kobj, &attr_group)) {
        kobject_put(calculator_kobj);
        return -ENOMEM;
    }

    printk(KERN_INFO "Calculator sysfs module loaded\n");
    return 0;
}

static void __exit calculator_sysfs_exit(void) {
    sysfs_remove_group(calculator_kobj, &attr_group);
    kobject_put(calculator_kobj);
    printk(KERN_INFO "Calculator sysfs module unloaded\n");
}

module_init(calculator_sysfs_init);
module_exit(calculator_sysfs_exit);
/*
# View the entries
ls /sys/kernel/calculator

# Do a calculation
echo "8 * 4" | sudo tee /sys/kernel/calculator/input

# See the result
cat /sys/kernel/calculator/result

# Optional: See last full input
cat /sys/kernel/calculator/input
*/
