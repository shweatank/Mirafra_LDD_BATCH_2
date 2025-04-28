#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timekeeping.h>
#include <linux/printk.h>

// Define custom log levels
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_ERROR 2

static int current_log_level = LOG_LEVEL_INFO;

// Function to set log level
void set_log_level(int level) {
    current_log_level = level;
}

// Helper function to convert log level to string
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
    const char *level_str = log_level_to_string(level);

    if (level < current_log_level) {
        return; // Skip logging if lower than current level
    }

    // Get current time
    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm);
    scnprintf(time_buf, sizeof(time_buf), "%04ld-%02d-%02d %02d:%02d:%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec);

    va_start(args, format);

    printk(KERN_INFO "[%s] [%s] ", level_str, time_buf);
    vprintk(format, args);
    printk("\n");

    va_end(args);
}

static int __init my_logger_init(void) {
    printk(KERN_INFO "Logger module loaded.\n");

    log_message(LOG_LEVEL_INFO, "This is an info message for entry");
    log_message(LOG_LEVEL_ERROR, "This is an error message with code %d", -1);

    return 0;
}

static void __exit my_logger_exit(void) {
    printk(KERN_INFO "Logger module unloaded.\n");
    log_message(LOG_LEVEL_INFO, "This is an info message for exit");
}

module_init(my_logger_init);
module_exit(my_logger_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("Kernel Logging Example");

