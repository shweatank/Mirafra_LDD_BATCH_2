#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>

#define DEVICE_NAME "lcd"
#define CLASS_NAME "lcd_class"

#define RS 26
#define EN 19
int data_pins[4] = {13, 6, 5, 11};  // D4-D7

static dev_t dev;
static struct cdev lcd_cdev;
static struct class *lcd_class;

// --- LCD Core ---
void lcd_pulse_enable(void) {
    gpio_set_value(EN, 1);
    udelay(1);
    gpio_set_value(EN, 0);
    udelay(100);
}

void lcd_send_nibble(unsigned char nibble) {
    for (int i = 0; i < 4; i++)
        gpio_set_value(data_pins[i], (nibble >> i) & 1);
    lcd_pulse_enable();
}

void lcd_send_byte(unsigned char byte, int is_data) {
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

void lcd_send_string(const char *str) {
    int i = 0;
    lcd_set_cursor(0, 0);
    while (i < 16 && *str)
        lcd_send_byte(*str++, 1), i++;

    if (*str) {
        lcd_set_cursor(1, 0);
        i = 0;
        while (i < 16 && *str)
            lcd_send_byte(*str++, 1), i++;
    }
}

void lcd_init(void) {
    gpio_set_value(RS, 0);
    lcd_send_nibble(0x03); mdelay(5);
    lcd_send_nibble(0x03); mdelay(1);
    lcd_send_nibble(0x03); lcd_send_nibble(0x02);  // 4-bit

    lcd_send_byte(0x28, 0); // 2-line, 5x7
    lcd_send_byte(0x0C, 0); // Display ON
    lcd_send_byte(0x06, 0); // Entry mode
    lcd_clear();
}

// --- Char Device File Ops ---
static ssize_t lcd_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    char kbuf[64] = {0};
    if (len > 63)
        len = 63;
    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    lcd_clear();
    lcd_send_string(kbuf);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = lcd_write,
};

// --- Init/Exit ---
static int __init lcd_init_driver(void) {
    int i;

    // Request GPIOs
    gpio_request(RS, "RS");
    gpio_request(EN, "EN");
    gpio_direction_output(RS, 0);
    gpio_direction_output(EN, 0);

    for (i = 0; i < 4; i++) {
        gpio_request(data_pins[i], "D");
        gpio_direction_output(data_pins[i], 0);
    }

    // LCD Init
    lcd_init();
    lcd_send_string("LCD Ready");

    // Char device registration
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&lcd_cdev, &fops);
    cdev_add(&lcd_cdev, dev, 1);

    lcd_class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(lcd_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "LCD: /dev/lcd created\n");
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

    printk(KERN_INFO "LCD: driver unloaded\n");
}

module_init(lcd_init_driver);
module_exit(lcd_exit_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI + You");
MODULE_DESCRIPTION("16x2 LCD with /dev/lcd char device on Raspberry Pi");

