#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/timekeeping.h>
#include <linux/uaccess.h>
#include <linux/printk.h>

#define DEVICE_NAME "read_data"
#define BUF_LEN 100
#define BUF_SIZE 4096

static struct file *filp = NULL;
static int major;
static char log_buffer[BUF_SIZE];
static int head = 0;
static int tail = 0;

// Log Levels
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_ERROR 2

static int current_log_level = LOG_LEVEL_INFO;

// Helper to set log level
void set_log_level(int level) {
    current_log_level = level;
}

// Convert level to string
static const char *log_level_to_string(int level) {
    switch (level) {
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_ERROR: return "ERROR";
        default:              return "UNKNOWN";
    }
}

// Save log to ring buffer
void save_log_to_buffer(const char *msg) {
    int i, len = strlen(msg);

    for (i = 0; i < len; i++) {
        log_buffer[head] = msg[i];
        head = (head + 1) % BUF_SIZE;
        if (head == tail) {  // Buffer full, overwrite
            tail = (tail + 1) % BUF_SIZE;
        }
    }
}

// Core logging function
void log_message(int level, const char *format, ...) {
    va_list args;
    struct timespec64 ts;
    struct tm tm;
    char time_buf[32];
    char log_buf_local[512];
    int len = 0;
    loff_t pos = 0;

    const char *level_str = log_level_to_string(level);

    if (level < current_log_level) {
        return;
    }

    // Timestamp
    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm);
    scnprintf(time_buf, sizeof(time_buf), "%04ld-%02d-%02d %02d:%02d:%02d",
              tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
              tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Format log
    va_start(args, format);
    len = scnprintf(log_buf_local, sizeof(log_buf_local), "[%s] [%s] ", level_str, time_buf);
    len += vsnprintf(log_buf_local + len, sizeof(log_buf_local) - len, format, args);
    len += scnprintf(log_buf_local + len, sizeof(log_buf_local) - len, "\n");
    va_end(args);

    printk(KERN_INFO "%s", log_buf_local);

    save_log_to_buffer(log_buf_local);

    if (filp) {
        kernel_write(filp, log_buf_local, len, &pos);
    }
}

// Device open
static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Logger Device opened\n");
    return 0;
}

// Device release
static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Logger Device closed\n");
    return 0;
}

// Device read
static ssize_t dev_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    int bytes_read = 0;

    while (tail != head && bytes_read < len) {
        if (put_user(log_buffer[tail], buf + bytes_read)) {
            return -EFAULT;
        }
        tail = (tail + 1) % BUF_SIZE;
        bytes_read++;
    }
    return bytes_read;
}

// Dummy write (optional)
static ssize_t dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "Logger device write not supported\n");
    return -EINVAL;
}

// File operations structure
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

// Module Init
static int __init my_logger_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Logger failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "Logger registered with major number %d\n", major);

    filp = filp_open("kernel_log.txt", O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (IS_ERR(filp)) {
        printk(KERN_ERR "Failed to open log file\n");
        filp = NULL;
    }

    log_message(LOG_LEVEL_INFO, "Logger Module Entry\n");
    log_message(LOG_LEVEL_ERROR, "Logger Module Test Error\n");

    return 0;
}

// Module Exit
static void __exit my_logger_exit(void) {
    if (filp) {
        filp_close(filp, NULL);
    }
    log_message(LOG_LEVEL_INFO, "Logger Module Exit\n");
    printk(KERN_INFO "Logger module unloaded\n");
    unregister_chrdev(major, DEVICE_NAME);
}

module_init(my_logger_init);
module_exit(my_logger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("Kernel Logger with User-Space Reading");

