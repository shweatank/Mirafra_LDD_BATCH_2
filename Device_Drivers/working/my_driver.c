#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "lcd_ioctl"
#define CLASS_NAME  "lcd"

#define LCD_IOCTL_MAGIC  'M'
#define LCD_IOCTL_LINE1  _IOW(LCD_IOCTL_MAGIC, 1, char *)
#define LCD_IOCTL_LINE2  _IOW(LCD_IOCTL_MAGIC, 2, char *)

#define I2C_LCD_ADDR     0x27
#define LCD_WIDTH        16

/* LCD Commands */
#define LCD_CMD          0
#define LCD_CHR          1

#define LCD_LINE_1       0x80
#define LCD_LINE_2       0xC0

#define LCD_BACKLIGHT    0x08
#define ENABLE           0x04

static struct i2c_client *lcd_client;

static struct class *lcd_class;
static struct cdev lcd_cdev;
static dev_t dev_num;

/* Strobe the enable bit to latch data */
static void lcd_strobe(uint8_t data)
{
    i2c_smbus_write_byte(lcd_client, data | ENABLE | LCD_BACKLIGHT);
    udelay(500);
    i2c_smbus_write_byte(lcd_client, (data & ~ENABLE) | LCD_BACKLIGHT);
    udelay(100);
}

/* Write command or data to LCD */
static void lcd_write(uint8_t bits, uint8_t mode)
{
    uint8_t high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
    uint8_t low  = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_smbus_write_byte(lcd_client, high);
    lcd_strobe(high);

    i2c_smbus_write_byte(lcd_client, low);
    lcd_strobe(low);
}

/* Initialize RG1602A LCD */
static void lcd_init_display(void)
{
    lcd_write(0x33, LCD_CMD); // Initialize
    lcd_write(0x32, LCD_CMD); // Set to 4-bit mode
    lcd_write(0x28, LCD_CMD); // 2 line, 5x7 matrix
    lcd_write(0x0C, LCD_CMD); // Display on, cursor off
    lcd_write(0x06, LCD_CMD); // Increment cursor
    lcd_write(0x01, LCD_CMD); // Clear display
    msleep(5);
    pr_info("RG1602A LCD Initialized\n");
}

/* Send message to LCD line */
static void lcd_message(const char *msg, uint8_t line)
{
    int i;

    lcd_write(line, LCD_CMD);
    for (i = 0; i < LCD_WIDTH && msg[i]; i++) {
        lcd_write(msg[i], LCD_CHR);
    }

    /* Fill rest of the line with spaces */
    while (i++ < LCD_WIDTH)
        lcd_write(' ', LCD_CHR);
}

/* ioctl handler to receive 2 lines of input from user space */
static long lcd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    char kbuf[LCD_WIDTH + 1] = {0};  // buffer for user data

    if (copy_from_user(kbuf, (char __user *)arg, LCD_WIDTH))
        return -EFAULT;

    switch (cmd) {
    case LCD_IOCTL_LINE1:
        lcd_message(kbuf, LCD_LINE_1);
        break;
    case LCD_IOCTL_LINE2:
        lcd_message(kbuf, LCD_LINE_2);
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = lcd_ioctl,
};

/* I2C probe function */
static int lcd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;

    pr_info("Probing RG1602A LCD I2C client\n");

    lcd_client = client;

    lcd_init_display();

    /* Allocate char device numbers */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate char device region\n");
        return ret;
    }

    /* Initialize cdev */
    cdev_init(&lcd_cdev, &fops);

    ret = cdev_add(&lcd_cdev, dev_num, 1);
    if (ret < 0) {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    lcd_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(lcd_class)) {
        pr_err("Failed to create class\n");
        cdev_del(&lcd_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(lcd_class);
    }

    device_create(lcd_class, NULL, dev_num, NULL, DEVICE_NAME);

    /* Display default messages */
    lcd_message("GOOD AFTERNOON!", LCD_LINE_1);
    lcd_message("HELLO VISHNU", LCD_LINE_2);

    pr_info("LCD device registered with major %d minor %d\n",
            MAJOR(dev_num), MINOR(dev_num));

    return 0;
}

/* I2C remove function */
static void lcd_i2c_remove(struct i2c_client *client)
{
    pr_info("Removing RG1602A LCD I2C client\n");

    lcd_write(0x01, LCD_CMD); // Clear display

    device_destroy(lcd_class, dev_num);
    class_destroy(lcd_class);
    cdev_del(&lcd_cdev);
    unregister_chrdev_region(dev_num, 1);

    return ;
}

static const struct i2c_device_id lcd_id[] = {
    { "rg1602a_lcd", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, lcd_id);

static const struct of_device_id lcd_of_match[] = {
    { .compatible = "vishnu,rg1602a-lcd" },
    { }
};
MODULE_DEVICE_TABLE(of, lcd_of_match);

static struct i2c_driver lcd_driver = {
    .driver = {
        .name = "rg1602a_lcd_driver",
        .of_match_table = lcd_of_match,
    },
    .probe = lcd_i2c_probe,
    .remove = lcd_i2c_remove,
    .id_table = lcd_id,
};

module_i2c_driver(lcd_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vishnu K");
MODULE_DESCRIPTION("I2C LCD Driver using probe/remove for RG1602A 16x2 display with ioctl support");

