#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

// DS3231 I2C address and register definitions
#define DS3231_I2C_ADDRESS 0x68
#define DS3231_REG_SECONDS 0x00
#define DS3231_REG_MINUTES 0x01
#define DS3231_REG_HOURS 0x02
#define DS3231_REG_DAY 0x03
#define DS3231_REG_DATE 0x04
#define DS3231_REG_MONTH 0x05
#define DS3231_REG_YEAR 0x06

// Convert BCD to decimal
static u8 bcd2dec(u8 bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Convert decimal to BCD
static u8 dec2bcd(u8 dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

// Read time from DS3231
static int ds3231_read_time(struct device *dev, struct rtc_time *tm)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 regs[7];
    int ret;

    // Read 7 bytes starting from seconds register
    ret = i2c_smbus_read_i2c_block_data(client, DS3231_REG_SECONDS, 7, regs);
    if (ret != 7) {
        dev_err(dev, "Failed to read time: %d\n", ret);
        return ret < 0 ? ret : -EIO;
    }

    tm->tm_sec = bcd2dec(regs[0] & 0x7F);
    tm->tm_min = bcd2dec(regs[1] & 0x7F);
    tm->tm_hour = bcd2dec(regs[2] & 0x3F); // 24-hour mode
    tm->tm_wday = bcd2dec(regs[3] & 0x07) - 1; // 1-7 to 0-6
    tm->tm_mday = bcd2dec(regs[4] & 0x3F);
    tm->tm_mon = bcd2dec(regs[5] & 0x1F) - 1; // 1-12 to 0-11
    tm->tm_year = bcd2dec(regs[6]) + 100; // Assume 20xx (100 = 2000 - 1900)

    return 0;
}

// Set time on DS3231
static int ds3231_set_time(struct device *dev, struct rtc_time *tm)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 regs[7];
    int ret;

    // Validate time
    if (tm->tm_sec < 0 || tm->tm_sec > 59 ||
        tm->tm_min < 0 || tm->tm_min > 59 ||
        tm->tm_hour < 0 || tm->tm_hour > 23 ||
        tm->tm_mday < 1 || tm->tm_mday > 31 ||
        tm->tm_mon < 0 || tm->tm_mon > 11 ||
        tm->tm_year < 100 || tm->tm_year > 199) {
        dev_err(dev, "Invalid time values\n");
        return -EINVAL;
    }

    // Convert to BCD
    regs[0] = dec2bcd(tm->tm_sec);
    regs[1] = dec2bcd(tm->tm_min);
    regs[2] = dec2bcd(tm->tm_hour);
    regs[3] = dec2bcd(tm->tm_wday + 1); // 0-6 to 1-7
    regs[4] = dec2bcd(tm->tm_mday);
    regs[5] = dec2bcd(tm->tm_mon + 1); // 0-11 to 1-12
    regs[6] = dec2bcd(tm->tm_year - 100); // 20xx to xx

    // Write to DS3231
    ret = i2c_smbus_write_i2c_block_data(client, DS3231_REG_SECONDS, 7, regs);
    if (ret < 0) {
        dev_err(dev, "Failed to set time: %d\n", ret);
        return ret;
    }

    return 0;
}

// RTC operations
static const struct rtc_class_ops ds3231_rtc_ops = {
    .read_time = ds3231_read_time,
    .set_time = ds3231_set_time,
};

// I2C device ID table
static const struct i2c_device_id ds3231_id[] = {
    { "ds3231", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ds3231_id);

// Device tree match table
static const struct of_device_id ds3231_of_match[] = {
    { .compatible = "maxim,ds3231" },
    { }
};
MODULE_DEVICE_TABLE(of, ds3231_of_match);

// I2C probe function
static int ds3231_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct rtc_device *rtc;
    int ret;

    // Verify I2C functionality
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_I2C_BLOCK)) {
        dev_err(&client->dev, "I2C adapter lacks SMBUS block support\n");
        return -ENODEV;
    }

    // Register RTC device
    rtc = devm_rtc_device_register(&client->dev, "ds3231", &ds3231_rtc_ops, THIS_MODULE);
    if (IS_ERR(rtc)) {
        dev_err(&client->dev, "Failed to register RTC device: %ld\n", PTR_ERR(rtc));
        return PTR_ERR(rtc);
    }

    i2c_set_clientdata(client, rtc);
    dev_info(&client->dev, "DS3231 RTC driver registered\n");

    return 0;
}

// I2C driver structure
static struct i2c_driver ds3231_driver = {
    .driver = {
        .name = "ds3231-rtc",
        .of_match_table = ds3231_of_match,
    },
    .probe = ds3231_probe,
    .id_table = ds3231_id,
};

// Module initialization
static int __init ds3231_init(void)
{
    return i2c_add_driver(&ds3231_driver);
}

// Module cleanup
static void __exit ds3231_exit(void)
{
    i2c_del_driver(&ds3231_driver);
}

module_init(ds3231_init);
module_exit(ds3231_exit);

MODULE_DESCRIPTION("DS3231 RTC Driver for Raspberry Pi");
MODULE_AUTHOR("Your Name");
MODULE_LICENSE("GPL");
