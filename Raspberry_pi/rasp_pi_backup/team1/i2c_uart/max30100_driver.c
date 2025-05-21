#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define MAX30102_I2C_ADDR     0x57
#define MAX30102_REG_PART_ID 0xFF
#define MAX30102_EXPECTED_ID 0x15
#define MAX30102_FIFO_DATA   0x07

static struct i2c_client *max30102_client;
static struct task_struct *reader_thread;

static int max30102_read_fifo(u32 *red, u32 *ir)
{
    u8 buf[6];
    struct i2c_msg msgs[] = {
        { .addr = max30102_client->addr, .flags = 0, .len = 1, .buf = buf },
        { .addr = max30102_client->addr, .flags = I2C_M_RD, .len = 6, .buf = buf }
    };

    buf[0] = MAX30102_FIFO_DATA;

    if (i2c_transfer(max30102_client->adapter, msgs, 2) != 2)
        return -EIO;

    *red = ((buf[0] & 0x03) << 16) | (buf[1] << 8) | buf[2];
    *ir  = ((buf[3] & 0x03) << 16) | (buf[4] << 8) | buf[5];

    return 0;
}

static int reader_fn(void *data)
{
    u32 red, ir;

    while (!kthread_should_stop()) {
        if (max30102_read_fifo(&red, &ir) == 0)
            pr_info("MAX30102: RED=%u, IR=%u\n", red, ir);
        else
            pr_warn("MAX30102: Read error\n");

        msleep(1000);
    }

    return 0;
}

static int max30102_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;
    u8 part_id;

    max30102_client = client;

    ret = i2c_smbus_read_byte_data(client, MAX30102_REG_PART_ID);
    if (ret < 0)
        return ret;

    part_id = ret;
    if (part_id != MAX30102_EXPECTED_ID) {
        pr_err("MAX30102: Unexpected part ID: 0x%02X\n", part_id);
        return -ENODEV;
    }

    pr_info("MAX30102: Sensor detected. Starting reader thread.\n");

    reader_thread = kthread_run(reader_fn, NULL, "max30102_reader");
    if (IS_ERR(reader_thread))
        return PTR_ERR(reader_thread);

    return 0;
}

static void max30102_remove(struct i2c_client *client)
{
    if (reader_thread)
        kthread_stop(reader_thread);
    pr_info("MAX30102: Driver removed.\n");
}

static const struct i2c_device_id max30102_id[] = {
    { "max30102", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, max30102_id);

static struct i2c_driver max30102_driver = {
    .driver = {
        .name = "max30102",
    },
    .probe    = max30102_probe,
    .remove   = max30102_remove,
    .id_table = max30102_id,
};

module_i2c_driver(max30102_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash R");
MODULE_DESCRIPTION("Kernel I2C Driver for MAX30102 Sensor");
