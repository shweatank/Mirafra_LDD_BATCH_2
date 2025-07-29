#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/bcd.h>

#define DS1307_REG_SEC    0x00
#define DS1307_REG_MIN    0x01
#define DS1307_REG_HOUR   0x02
#define DS1307_REG_DAY    0x03
#define DS1307_REG_DATE   0x04
#define DS1307_REG_MONTH  0x05
#define DS1307_REG_YEAR   0x06

static int ds1307_read_time(struct device *dev, struct rtc_time *tm)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 data[7];
    int ret;

    ret = i2c_smbus_read_i2c_block_data(client, DS1307_REG_SEC, 7, data);
    if (ret < 0)
        return ret;

    tm->tm_sec  = bcd2bin(data[0] & 0x7F);
    tm->tm_min  = bcd2bin(data[1] & 0x7F);
    tm->tm_hour = bcd2bin(data[2] & 0x3F);
    tm->tm_mday = bcd2bin(data[4] & 0x3F);
    tm->tm_mon  = bcd2bin(data[5] & 0x1F) - 1;
    tm->tm_year = bcd2bin(data[6]) + 100;
    tm->tm_wday = bcd2bin(data[3] & 0x07) - 1;

    dev_info(dev, "Read time: %ptR\n", tm);
    return 0;
}

static int ds1307_set_time(struct device *dev, struct rtc_time *tm)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 data[7];

    data[0] = bin2bcd(tm->tm_sec);
    data[1] = bin2bcd(tm->tm_min);
    data[2] = bin2bcd(tm->tm_hour);
    data[3] = bin2bcd(tm->tm_wday + 1);
    data[4] = bin2bcd(tm->tm_mday);
    data[5] = bin2bcd(tm->tm_mon + 1);
    data[6] = bin2bcd(tm->tm_year - 100);

    return i2c_smbus_write_i2c_block_data(client, DS1307_REG_SEC, 7, data);
}

static const struct rtc_class_ops ds1307_ops = {
    .read_time = ds1307_read_time,
    .set_time  = ds1307_set_time,
};

static int ds1307_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct rtc_device *rtc;

    rtc = devm_rtc_device_register(&client->dev, "ds1307",
                                   &ds1307_ops, THIS_MODULE);
    if (IS_ERR(rtc))
        return PTR_ERR(rtc);

    dev_info(&client->dev, "DS1307 RTC driver loaded\n");
    return 0;
}

static const struct i2c_device_id ds1307_id[] = {
    { "ds1307", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ds1307_id);

static const struct of_device_id ds1307_of_match[] = {
    { .compatible = "dallas,ds1307" },
    { }
};
MODULE_DEVICE_TABLE(of, ds1307_of_match);

static struct i2c_driver ds1307_driver = {
    .driver = {
        .name = "ds1307",
        .of_match_table = ds1307_of_match,
    },
    .probe    = ds1307_probe,
    .id_table = ds1307_id,
};

module_i2c_driver(ds1307_driver);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEJA");
MODULE_DESCRIPTION("DS1307 I2C RTC Driver");

