#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/timekeeping.h>
#include <linux/uaccess.h>
#include <linux/printk.h>

static struct file *filp = NULL;

// Define custom log levels
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_ERROR 2

static int current_log_level = LOG_LEVEL_INFO;

// Helper to set log level
void set_log_level(int level) {
    current_log_level = level;
}

// Helper to convert log level to string
static const char *log_level_to_string(int level) {
    switch (level) {
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_ERROR: return "ERROR";
        default:              return "UNKNOWN";
    }
}

// Core logging function
void log_message(int level, const char *format, ...) {
    va_list args;
    struct timespec64 ts;
    struct tm tm;
    char time_buf[32];
    char log_buf[512];
    int len = 0;
    loff_t pos = 0;  // Position for kernel_write

    const char *level_str = log_level_to_string(level);

    if (level < current_log_level) {
        return;
    }

    // Get current time
    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm);
    scnprintf(time_buf, sizeof(time_buf), "%04ld-%02d-%02d %02d:%02d:%02d",
              tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
              tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Prepare the formatted log string
    va_start(args, format);
    len = scnprintf(log_buf, sizeof(log_buf), "[%s] [%s] ", level_str, time_buf);
    len += vsnprintf(log_buf + len, sizeof(log_buf) - len, format, args);
    len += scnprintf(log_buf + len, sizeof(log_buf) - len, "\n");
    va_end(args);

    // Print to kernel log
    printk(KERN_INFO "%s", log_buf);

    // Write to file if available
    if (filp) {
        kernel_write(filp, log_buf, len, &pos);
    }
}

static int __init my_logger_init(void) {
    printk(KERN_INFO "Logger module loaded.\n");

    // Open the file during module load
    filp = filp_open("kernel_log.txt", O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (IS_ERR(filp)) {
        printk(KERN_ERR "Failed to open log file\n");
        filp = NULL;
    }

    log_message(LOG_LEVEL_INFO, "This is an info message for entry");
    log_message(LOG_LEVEL_ERROR, "This is an error message with code %d", -1);

    return 0;
}

static void __exit my_logger_exit(void) {
    log_message(LOG_LEVEL_INFO, "This is an info message for exit");

    // Close the file during module unload
    if (filp) {
        filp_close(filp, NULL);
    }

    printk(KERN_INFO "Logger module unloaded.\n");
}

module_init(my_logger_init);
module_exit(my_logger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("Kernel Logging Example Writing To File");

