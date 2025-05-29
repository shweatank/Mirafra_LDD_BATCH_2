#include <linux/module.h>   
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/timekeeping.h>
#include <linux/printk.h>
#include <linux/kernel.h>

#define DEVICE_NAME "lcd"
#define CLASS_NAME "lcd_class"

#define RS 26
#define EN 19
int data_pins[4] = {13, 6, 5, 11};  // D4-D7  (gpio-pin)

static dev_t dev;
static struct cdev lcd_cdev;
static struct class *lcd_class;

// ---- Logging Section ----
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_ERROR 2
static int current_log_level = LOG_LEVEL_INFO;

static struct file *filp = NULL;

void set_log_level(int level) {
    current_log_level = level;
}

static const char *log_level_to_string(int level) {
    switch (level) {
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_ERROR: return "ERROR";
        default:              return "UNKNOWN";
    }
}

void log_message(int level, const char *format, ...) {
    va_list args;
    struct timespec64 ts;
    struct tm tm;
    char time_buf[32];
    char log_buf[512];
    int len = 0;
    loff_t pos = 0;

    if (level < current_log_level)
        return;

    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm);
    scnprintf(time_buf, sizeof(time_buf), "%04ld-%02d-%02d %02d:%02d:%02d",
              tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
              tm.tm_hour, tm.tm_min, tm.tm_sec);

    va_start(args, format);
    len = scnprintf(log_buf, sizeof(log_buf), "[%s] [%s] ", log_level_to_string(level), time_buf);
    len += vsnprintf(log_buf + len, sizeof(log_buf) - len, format, args);
    len += scnprintf(log_buf + len, sizeof(log_buf) - len, "\n");
    va_end(args);

    printk(KERN_INFO "%s", log_buf);

    if (filp)
        kernel_write(filp, log_buf, len, &pos);  //write mess to a timestamp and level in a file
}
// ---------------------------


// --- LCD Core ---
void lcd_pulse_enable(void) {
    gpio_set_value(EN, 1);  //latch on -off
    udelay(1);
    gpio_set_value(EN, 0);
    udelay(100);
}

void lcd_send_nibble(unsigned char nibble) {     //sends a 4-bit nibble to the LCD data pins and then pulses the enable pin to latch the data.
    for (int i = 0; i < 4; i++)
        gpio_set_value(data_pins[i], (nibble >> i) & 1);
    lcd_pulse_enable();
}

void lcd_send_byte(unsigned char byte, int is_data) {   //This function sends a full byte (8 bits) by splitting it into two nibbles.
    gpio_set_value(RS, is_data);
    lcd_send_nibble(byte >> 4);
    lcd_send_nibble(byte & 0x0F);
    mdelay(2);
}

void lcd_set_cursor(int row, int col) {
    int addr = (row == 0) ? 0x00 : 0x40;
    lcd_send_byte(0x80 | (addr + col), 0);
}

void lcd_clear(void) {
    lcd_send_byte(0x01, 0);
    mdelay(5);
}

void lcd_send_string(const char *str) {    //print the string
    int i = 0;
    lcd_set_cursor(0, 0);   //row and colm
    while (i < 16 && *str)     //first line
        lcd_send_byte(*str++, 1), i++;

    if (*str) {
        lcd_set_cursor(1, 0);   //2nd line
        i = 0;
        while (i < 16 && *str)
            lcd_send_byte(*str++, 1), i++;
    }
}

void lcd_init(void) {
    gpio_set_value(RS, 0);   //command not a data
    lcd_send_nibble(0x03); mdelay(5);  //) resets the LCD controller to 8-bit mode
    lcd_send_nibble(0x03); mdelay(1);
    lcd_send_nibble(0x03); lcd_send_nibble(0x02);  // 4-bit mode

    lcd_send_byte(0x28, 0); // 2-line, 5x7(5*8 dots)
    lcd_send_byte(0x0C, 0); // Display ON
    lcd_send_byte(0x06, 0); // Entry mode
    lcd_clear();

    log_message(LOG_LEVEL_INFO, "LCD initialized");
}

// --- Char Device File Ops ---
static ssize_t lcd_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    char kbuf[64] = {0};
    if (len > 63)
        len = 63;

    if (copy_from_user(kbuf, buf, len)) {   //buf---> kbuff
        log_message(LOG_LEVEL_ERROR, "Failed to copy data from user");
        return -EFAULT;
    }

    lcd_clear();
    lcd_send_string(kbuf);   ..send kbuf to lcd

    log_message(LOG_LEVEL_INFO, "LCD message written: %s", kbuf);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = lcd_write,
};

// --- Init/Exit ---
static int __init lcd_init_driver(void) {
    int i;

    log_message(LOG_LEVEL_INFO, "Loading LCD driver...");

    // Open log file
    filp = filp_open("kernel_lcd_log.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (IS_ERR(filp)) {
        log_message(LOG_LEVEL_ERROR, "Failed to open log file");
        filp = NULL;
    }

    if (gpio_request(RS, "RS") || gpio_request(EN, "EN")) {
        log_message(LOG_LEVEL_ERROR, "Failed to request RS or EN GPIOs");
        return -EBUSY;
    }

    gpio_direction_output(RS, 0);
    gpio_direction_output(EN, 0);

    for (i = 0; i < 4; i++) {
        if (gpio_request(data_pins[i], "D")) {
            log_message(LOG_LEVEL_ERROR, "Failed to request GPIO %d", data_pins[i]);
            return -EBUSY;
        }
        gpio_direction_output(data_pins[i], 0);
    }

    lcd_init();
    lcd_send_string("LCD Ready");

    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&lcd_cdev, &fops);
    cdev_add(&lcd_cdev, dev, 1);

    lcd_class = class_create(CLASS_NAME);   ///sys/class/  used by /dev creation
    device_create(lcd_class, NULL, dev, NULL, DEVICE_NAME);   ///dev/lcd

    log_message(LOG_LEVEL_INFO, "LCD driver loaded: /dev/lcd ready");
    return 0;
}

static void __exit lcd_exit_driver(void) {
    int i;

    lcd_clear();
    device_destroy(lcd_class, dev);
    class_destroy(lcd_class);
    cdev_del(&lcd_cdev);
    unregister_chrdev_region(dev, 1);

    for (i = 0; i < 4; i++)
        gpio_free(data_pins[i]);

    gpio_free(RS);
    gpio_free(EN);

    if (filp)
        filp_close(filp, NULL);

    log_message(LOG_LEVEL_INFO, "LCD driver unloaded");
}

module_init(lcd_init_driver);
module_exit(lcd_exit_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("16x2 LCD with Logging via /dev/lcd");

