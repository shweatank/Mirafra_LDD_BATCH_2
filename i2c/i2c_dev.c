#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "my_i2c_dev"
#define I2C_SLAVE_ADDRESS 0x50  // Replace with your device's address

static struct i2c_client *my_i2c_client;
static int major;

static int my_open(struct inode *inode, struct file *file) {
    pr_info("i2c: Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    pr_info("i2c: Device closed\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *offset) {
    char data;
    int ret = i2c_smbus_read_byte_data(my_i2c_client, 0x00);
    if (ret < 0) return ret;

    data = ret;
    if (copy_to_user(buf, &data, 1)) return -EFAULT;

    return 1;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *offset) {
    char data;
    if (copy_from_user(&data, buf, 1)) return -EFAULT;

    return i2c_smbus_write_byte_data(my_i2c_client, 0x00, data);
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_release,
    .read    = my_read,
    .write   = my_write,
};

// Called when a device matching the i2c_device_id is found
static int my_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    pr_info("i2c: Probing device 0x%x\n", client->addr);
    my_i2c_client = client;

    major = register_chrdev(0, DRIVER_NAME, &fops);
    if (major < 0) {
        pr_err("Failed to register char device\n");
        return major;
    }

    pr_info("Char device registered with major %d\n", major);
    return 0;
}

static int my_i2c_remove(struct i2c_client *client) {
    unregister_chrdev(major, DRIVER_NAME);
    pr_info("i2c: Device removed\n");
    return 0;
}

static const struct i2c_device_id my_i2c_id[] = {
    { "my_i2c_device", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, my_i2c_id);

static struct i2c_driver my_i2c_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe    = my_i2c_probe,
    .remove   = my_i2c_remove,
    .id_table = my_i2c_id,
};

module_i2c_driver(my_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Simple I2C Client Driver");
