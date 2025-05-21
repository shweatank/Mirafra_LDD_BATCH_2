#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>  // For copy_from_user and copy_to_user
#include <linux/string.h>    // For strtol

#define PROC_NAME "calculator"
//echo "3 + 5" > /proc/calculator
//cat /proc/calculator
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple calculator kernel module using Procfs");
MODULE_VERSION("0.1");

static char input[256] = {0}; // Buffer to hold input from user
static int result = 0;        // Result of the operation

// Function to perform basic arithmetic operations
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) {
    if (b == 0) {
        printk(KERN_ALERT "Error: Division by zero\n");
        return -1;  // Error value for division by zero
    }
    return a / b;
}

// Function to process the arithmetic operation written to the proc file
void calculate(char *expression) {
    int a, b;
    char operation;

    // Parse the input string (e.g., "3 + 5")
    if (sscanf(expression, "%d %c %d", &a, &operation, &b) == 3) {
        switch (operation) {
            case '+':
                result = add(a, b);
                break;
            case '-':
                result = subtract(a, b);
                break;
            case '*':
                result = multiply(a, b);
                break;
            case '/':
                result = divide(a, b);
                break;
            default:
                printk(KERN_ALERT "Unknown operation: %c\n", operation);
                result = -1;
        }
        printk(KERN_INFO "Operation: %d %c %d = %d\n", a, operation, b, result);
    } else {
        printk(KERN_ALERT "Invalid input format. Expected: <num> <operation> <num>\n");
    }
}

// Read function for the proc file
static ssize_t procfile_read(struct file *file, char __user *buffer, size_t length, loff_t *offset) {
    char result_str[256];

    // Prepare the result string to send to the user
    snprintf(result_str, sizeof(result_str), "Result: %d\n", result);

    // Copy the result to user space
    if (*offset > 0)
        return 0;

    if (copy_to_user(buffer, result_str, strlen(result_str))) {
        return -EFAULT;
    }

    *offset += strlen(result_str);
    return strlen(result_str);
}

// Write function for the proc file
static ssize_t procfile_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset) {
    if (length > sizeof(input) - 1)
        return -EINVAL;

    // Copy the input from the user to the kernel space
    if (copy_from_user(input, buffer, length))
        return -EFAULT;

    input[length] = '\0';  // Null-terminate the string

    // Perform the calculation
    calculate(input);

    return length;
}

// Define the proc_ops structure for the new kernel version
static const struct proc_ops proc_fops = {
    .proc_read = procfile_read,
    .proc_write = procfile_write,
};

// Module initialization
static int __init calculator_init(void) {
    // Create the /proc file
    struct proc_dir_entry *entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    if (!entry) {
        printk(KERN_ALERT "Failed to create /proc entry\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "Calculator ProcFS module loaded\n");
    return 0;
}

// Module exit
static void __exit calculator_exit(void) {
    // Remove the /proc file
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "Calculator ProcFS module unloaded\n");
}

module_init(calculator_init);
module_exit(calculator_exit);

