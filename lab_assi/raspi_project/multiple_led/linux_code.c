#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define MAX_LEDS 10

static int gpio_pins[MAX_LEDS];
static int num_leds;
static struct task_struct *blink_thread;

static int blink_func(void *data)
{
    int i;
    while (!kthread_should_stop()) {
        for (i = 0; i < num_leds; i++)
            gpio_set_value(gpio_pins[i], 1);

        msleep(500);

        for (i = 0; i < num_leds; i++)
            gpio_set_value(gpio_pins[i], 0);

        msleep(500);
    }
    return 0;
}

static int multi_led_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;
    int i, ret;

    num_leds = of_gpio_named_count(np, "led-gpios");
    if (num_leds <= 0 || num_leds > MAX_LEDS) {
        dev_err(dev, "Invalid number of GPIOs: %d\n", num_leds);
        return -EINVAL;
    }

    for (i = 0; i < num_leds; i++) {
        gpio_pins[i] = of_get_named_gpio(np, "led-gpios", i);
        if (!gpio_is_valid(gpio_pins[i])) {
            dev_err(dev, "Invalid GPIO at index %d\n", i);
            return -EINVAL;
        }

        ret = gpio_request(gpio_pins[i], "multi_led");
        if (ret) {
            dev_err(dev, "GPIO request failed at index %d\n", i);
            return ret;
        }

        gpio_direction_output(gpio_pins[i], 0);
    }

    blink_thread = kthread_run(blink_func, NULL, "led_blinker_thread");
    if (IS_ERR(blink_thread)) {
        dev_err(dev, "Failed to create thread\n");
        for (i = 0; i < num_leds; i++)
            gpio_free(gpio_pins[i]);
        return PTR_ERR(blink_thread);
    }

    dev_info(dev, "Multi-LED driver loaded successfully\n");
    return 0;
}

static int multi_led_remove(struct platform_device *pdev)
{
    int i;

    kthread_stop(blink_thread);

    for (i = 0; i < num_leds; i++) {
        gpio_set_value(gpio_pins[i], 0);
        gpio_free(gpio_pins[i]);
    }

    pr_info("Multi-LED driver removed\n");
    return 0;
}

static const struct of_device_id multi_led_of_match[] = {
    { .compatible = "techdhaba,multi-led-blink" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, multi_led_of_match);

static struct platform_driver multi_led_driver = {
    .probe = multi_led_probe,
    .remove = multi_led_remove,
    .driver = {
        .name = "multi-led-driver",
        .of_match_table = multi_led_of_match,
    },
};

module_platform_driver(multi_led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Multi-LED Blinking Driver for Raspberry Pi");
