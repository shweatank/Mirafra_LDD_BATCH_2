#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>

static int gpio_pin;
static struct task_struct *blink_thread;

static int led_blink_thread(void *data) {
    while (!kthread_should_stop()) {
        gpio_set_value(gpio_pin, 1);
        msleep(500);
        gpio_set_value(gpio_pin, 0);
        msleep(500);
    }
    return 0;
}

static int led_probe(struct platform_device *pdev) {
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;

    gpio_pin = of_get_named_gpio(np, "gpios", 0);
    if (!gpio_is_valid(gpio_pin)) {
        dev_err(dev, "Invalid GPIO\n");
        return -EINVAL;
    }

    if (gpio_request(gpio_pin, "led_gpio")) {
        dev_err(dev, "GPIO request failed\n");
        return -EBUSY;
    }

    gpio_direction_output(gpio_pin, 0);

    blink_thread = kthread_run(led_blink_thread, NULL, "led_blink");
    if (IS_ERR(blink_thread)) {
        gpio_free(gpio_pin);
        return PTR_ERR(blink_thread);
    }

    dev_info(dev, "LED driver loaded and blinking started.\n");
    return 0;
}

static int led_remove(struct platform_device *pdev) {
    kthread_stop(blink_thread);
    gpio_set_value(gpio_pin, 0);
    gpio_free(gpio_pin);
    pr_info("LED driver unloaded.\n");
    return 0;
}

static const struct of_device_id led_dt_ids[] = {
    { .compatible = "techdhaba,led-blink" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, led_dt_ids);

static struct platform_driver led_driver = {
    .driver = {
        .name = "led-blinker",
        .of_match_table = led_dt_ids,
    },
    .probe = led_probe,
    .remove = led_remove,
};

module_platform_driver(led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Custom LED Blink Driver for Raspberry Pi using DTS");
