// i2c_driver.c
#include <linux/module.h>
#include <linux/i2c.h>

static int i2c_demo_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    pr_info("I2C device probed: %s\n", client->name);
    return 0;
}

static int i2c_demo_remove(struct i2c_client *client) {
    pr_info("I2C device removed\n");
    return 0;
}

static const struct i2c_device_id i2c_demo_id[] = {
    { "i2c_demo", 0 },
    {}
};
MODULE_DEVICE_TABLE(i2c, i2c_demo_id);

static struct i2c_driver i2c_demo_driver = {
    .driver = {
        .name = "i2c_demo",
    },
    .probe = i2c_demo_probe,
    .remove = i2c_demo_remove,
    .id_table = i2c_demo_id,
};

module_i2c_driver(i2c_demo_driver);
MODULE_LICENSE("GPL");
