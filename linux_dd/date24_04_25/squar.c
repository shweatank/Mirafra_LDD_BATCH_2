#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "charcalc"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static double result;  // Use double for more precision in square root results

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "charcalc: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "charcalc: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char result_msg[BUF_LEN];
    int msg_len = snprintf(result_msg, BUF_LEN, "Result: %.5f\n", result);  // Show result with 5 decimal places
    int error_count = copy_to_user(buffer, result_msg, msg_len);

    return error_count == 0 ? msg_len : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    int a;  // Operand for the square root calculation

    if (copy_from_user(message, buffer, len)) {
        return -EFAULT;
    }

    message[len] = '\0'; // Null-terminate the string

    // Parse input, expecting one integer (a), for square root calculation
    if (sscanf(message, "%d", &a) != 1) {
        printk(KERN_ERR "charcalc: Invalid input format\n");
        return -EINVAL;
    }

    if (a < 0) {
        printk(KERN_ERR "charcalc: Negative input for square root\n");
        return -EINVAL;  // Return error for negative number
    }

    if (a == 0) {
        result = 0.0;  // The square root of 0 is 0
    } else {
        // Binary search for square root calculation
        int low = 0, high = a;
        int mid;
        int epsilon = 0; // Accuracy of the result

        // Perform binary search to find square root
        while ((high - low) > epsilon) {
            mid = (low + high) / 2;

            if (mid * mid > a) {
                high = mid;
            } else {
                low = mid;
            }
        }
        result = mid;  // Store the square root result
    }

    printk(KERN_INFO "charcalc: Square root of %d = %.5f\n", a, result);  // Print result with 5 decimal places
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init charcalc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "charcalc failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "charcalc: registered with major number %d\n", major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "charcalc: unregistered\n");
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple character device that performs square root calculations");

