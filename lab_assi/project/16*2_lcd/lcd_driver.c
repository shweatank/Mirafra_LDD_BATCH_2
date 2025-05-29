#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/init.h>

#define RS 26
#define EN 19
int data_pins[4] = {13, 6, 5, 11};  // D4-D7

void lcd_pulse_enable(void) {
    gpio_set_value(EN, 1);
    udelay(1);
    gpio_set_value(EN, 0);
    udelay(100);
}

void lcd_send_nibble(unsigned char nibble) {
    int i;
    for (i = 0; i < 4; i++)
        gpio_set_value(data_pins[i], (nibble >> i) & 1);
    lcd_pulse_enable();
}

void lcd_send_byte(unsigned char byte, int is_data) {
    gpio_set_value(RS, is_data);
    lcd_send_nibble(byte >> 4);
    lcd_send_nibble(byte & 0x0F);
    mdelay(2);
}

void lcd_init(void) {
    gpio_set_value(RS, 0);
    lcd_send_nibble(0x03);
    mdelay(5);
    lcd_send_nibble(0x03);
    mdelay(1);
    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02); // 4-bit mode

    lcd_send_byte(0x28, 0); // 2-line, 5x7
    lcd_send_byte(0x0C, 0); // Display ON
    lcd_send_byte(0x06, 0); // Entry mode
    lcd_send_byte(0x01, 0); // Clear display
}

void lcd_send_string(const char *str) {
    while (*str)
        lcd_send_byte(*str++, 1);
}

static int __init lcd_driver_init(void) {
    int i;

    printk(KERN_INFO "LCD: Initializing 16x2 LCD...\n");

    gpio_request(RS, "RS");
    gpio_request(EN, "EN");
    gpio_direction_output(RS, 0);
    gpio_direction_output(EN, 0);

    for (i = 0; i < 4; i++) {
        gpio_request(data_pins[i], "D");
        gpio_direction_output(data_pins[i], 0);
    }

    lcd_init();
    lcd_send_string("Hello, Kernel!");

    return 0;
}

static void __exit lcd_driver_exit(void) {
    int i;
    lcd_send_byte(0x01, 0); // Clear display
    gpio_set_value(EN, 0);
    gpio_set_value(RS, 0);

    for (i = 0; i < 4; i++)
        gpio_free(data_pins[i]);

    gpio_free(RS);
    gpio_free(EN);

    printk(KERN_INFO "LCD: Module unloaded\n");
}

module_init(lcd_driver_init);
module_exit(lcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("16x2 LCD Display Driver using GPIO on Raspberry Pi");

