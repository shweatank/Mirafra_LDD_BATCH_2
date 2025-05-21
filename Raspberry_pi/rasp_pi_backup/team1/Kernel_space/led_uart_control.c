#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#define LED_GPIO 17  // GPIO17 (Pin 11 on Pi)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash R");
MODULE_DESCRIPTION("Control LED brightness from user-space via /dev/led_uart");

static int brightness_level = 0;

static void blink_led(int level)
{
    int i;
    for (i = 0; i < level * 10; i++) {
        gpio_set_value(LED_GPIO, 1);
        udelay(500);
        gpio_set_value(LED_GPIO, 0);
        udelay(500);
    }
}

static ssize_t led_uart_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    char ch;

    if (copy_from_user(&ch, buf, 1))
        return -EFAULT;

    if (ch >= '1' && ch <= '5') {
        brightness_level = ch - '0';
        pr_info("LED: Brightness level set to %d\n", brightness_level);
        blink_led(brightness_level);
    } else {
        pr_warn("LED: Invalid brightness input '%c'\n", ch);
    }

    return 1;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = led_uart_write,
};

static struct miscdevice led_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "led_uart",
    .fops = &fops,
    .mode = 0666,
};

static int __init led_uart_init(void)
{
    int ret;

    ret = gpio_request(LED_GPIO, "led_gpio");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", LED_GPIO);
        return ret;
    }

    gpio_direction_output(LED_GPIO, 0);

    ret = misc_register(&led_misc);
    if (ret) {
        gpio_free(LED_GPIO);
        pr_err("Failed to register misc device\n");
        return ret;
    }

    pr_info("led_uart kernel module loaded. Use /dev/led_uart\n");
    return 0;
}

static void __exit led_uart_exit(void)
{
    misc_deregister(&led_misc);
    gpio_set_value(LED_GPIO, 0);
    gpio_free(LED_GPIO);
    pr_info("led_uart kernel module unloaded\n");
}

module_init(led_uart_init);
module_exit(led_uart_exit);
