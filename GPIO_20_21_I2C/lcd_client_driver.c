#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define I2C_BUS_AVAILABLE   5          // Your custom I2C adapter (e.g., from mmio)
#define SLAVE_DEVICE_NAME   "lcd1602"    // Device Name
#define LCD_SLAVE_ADDR      0x27         // I2C address of LCD (change if needed)

static struct i2c_adapter *lcd_adapter     = NULL;
static struct i2c_client  *lcd_client      = NULL;

/* Write byte to LCD over I2C */
static int lcd_i2c_write_byte(uint8_t val)
{
    return i2c_master_send(lcd_client, &val, 1);
}

/* Low level send nibble with backlight and EN pin pulse */
static void lcd_send_nibble(uint8_t nibble)
{
    // PCF8574: D7-D4 | EN | RW | RS | Backlight (bit 3~7 important)
    uint8_t data = nibble | 0x08; // Backlight ON

    lcd_i2c_write_byte(data | 0x04); // EN=1
    udelay(500);
    lcd_i2c_write_byte(data & ~0x04); // EN=0
    udelay(500);
}

/* Send command or data to LCD (RS=0 -> cmd, RS=1 -> data) */
static void lcd_send(uint8_t val, bool is_data)
{
    uint8_t high = (val & 0xF0);
    uint8_t low  = ((val << 4) & 0xF0);

    if (is_data) {
        high |= 0x01;  // RS = 1
        low  |= 0x01;
    }

    lcd_send_nibble(high);
    lcd_send_nibble(low);
}

/* Send command */
static void lcd_cmd(uint8_t cmd)
{
    lcd_send(cmd, false);
}

/* Send char */
static void lcd_data(uint8_t data)
{
    lcd_send(data, true);
}

/* LCD init sequence for 16x2 HD44780 */
static void lcd_init_sequence(void)
{
    msleep(50);
    lcd_send_nibble(0x30);
    msleep(5);
    lcd_send_nibble(0x30);
    udelay(150);
    lcd_send_nibble(0x30);
    udelay(150);
    lcd_send_nibble(0x20); // Set to 4-bit mode
    udelay(150);

    lcd_cmd(0x28); // 4-bit, 2 line, 5x8 font
    lcd_cmd(0x08); // Display off
    lcd_cmd(0x01); // Clear display
    msleep(2);
    lcd_cmd(0x06); // Entry mode set
    lcd_cmd(0x0C); // Display on, cursor off
}

/* Print a string to LCD */
static void lcd_print(const char *str)
{
    while (*str) {
        lcd_data(*str++);
    }
}

/* I2C probe */
static int lcd1602_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    lcd_client = client;
    lcd_init_sequence();
    lcd_cmd(0x80);  // Set cursor to 1st line
    lcd_print("Hello, Mohan!");
    lcd_cmd(0xC0);  // Set cursor to 2nd line
    lcd_print("LCD I2C Ready");
    pr_info("LCD Client Probed\n");
    return 0;
}

static void lcd1602_remove(struct i2c_client *client)
{
    lcd_cmd(0x01); // Clear display
    pr_info("LCD Client Removed\n");
}

/* I2C device ID table */
static const struct i2c_device_id lcd1602_id[] = {
    { SLAVE_DEVICE_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, lcd1602_id);

/* I2C driver struct */
static struct i2c_driver lcd1602_driver = {
    .driver = {
        .name   = SLAVE_DEVICE_NAME,
        .owner  = THIS_MODULE,
    },
    .probe    = lcd1602_probe,
    .remove   = lcd1602_remove,
    .id_table = lcd1602_id,
};

/* Create i2c_client using i2c_new_client_device */
static struct i2c_board_info lcd_i2c_board_info = {
    I2C_BOARD_INFO(SLAVE_DEVICE_NAME, LCD_SLAVE_ADDR)
};

/* Module init */
static int __init lcd_driver_init(void)
{
    int ret = -1;
    lcd_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
    if (!lcd_adapter) {
        pr_err("Failed to get adapter\n");
        return -ENODEV;
    }

    lcd_client = i2c_new_client_device(lcd_adapter, &lcd_i2c_board_info);
    if (!lcd_client) {
        pr_err("Failed to create I2C client\n");
        i2c_put_adapter(lcd_adapter);
        return -ENODEV;
    }

    ret = i2c_add_driver(&lcd1602_driver);
    i2c_put_adapter(lcd_adapter);
    return ret;
}

/* Module exit */
static void __exit lcd_driver_exit(void)
{
    i2c_unregister_device(lcd_client);
    i2c_del_driver(&lcd1602_driver);
}

module_init(lcd_driver_init);
module_exit(lcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohan");
MODULE_DESCRIPTION("HD44780 16x2 LCD I2C client driver");

