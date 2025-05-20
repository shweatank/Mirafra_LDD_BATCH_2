#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEVICE_NAME "i2c_sensor"
#define CLASS_NAME  "i2c_sensor_class"

static struct i2c_client *sensor_client;
static struct class* sensor_class = NULL;
static struct device* sensor_device = NULL;
static int major;

static int sensor_open(struct inode *inode, struct file *file)
{
    pr_info("i2c_sensor: Device opened\n");
    return 0;
}

int sensor_release(struct inode *inode, struct file *file)
{
    pr_info("i2c_sensor: Device closed\n");
    return 0;
}

static ssize_t sensor_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    int ret;
    u8 data[2];  // adjust based on sensor register width

    // Example: read 2 bytes from register 0x00
    struct i2c_msg msgs[] = {
        {
            .addr = sensor_client->addr,
            .flags = 0,
            .len = 1,
            .buf = (u8[]){ 0x00 }, // register address
        },
        {
            .addr = sensor_client->addr,
            .flags = I2C_M_RD,
            .len = 2,
            .buf = data,
        }
    };

    ret = i2c_transfer(sensor_client->adapter, msgs, 2);
    if (ret < 0) {
        pr_err("i2c_sensor: Failed to read from sensor\n");
        return ret;
    }

    // Send data to user
    if (copy_to_user(buf, data, 2))
        return -EFAULT;

    return 2;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = sensor_open,
    .release = sensor_release,
    .read = sensor_read,
};

static int sensor_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("i2c_sensor: Probing device\n");
    sensor_client = client;

    major = register_chrdev(0, DEVICE_NAME, &fops);
    sensor_class = class_create(THIS_MODULE, CLASS_NAME);
    sensor_device = device_create(sensor_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    return 0;
}

void sensor_remove(struct i2c_client *client)
{
    pr_info("i2c_sensor: Removing driver\n");
    device_destroy(sensor_class, MKDEV(major, 0));
    class_destroy(sensor_class);
    unregister_chrdev(major, DEVICE_NAME);
    return ;
}

static const struct i2c_device_id sensor_id[] = {
    { "i2c_sensor", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, sensor_id);

static struct i2c_driver sensor_driver = {
    .driver = {
        .name = "i2c_sensor_driver",
        .owner = THIS_MODULE,
    },
    .probe = sensor_probe,
    .remove = sensor_remove,
    .id_table = sensor_id,
};

module_i2c_driver(sensor_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("Simple I2C Sensor Driver");
MODULE_VERSION("1.0");

