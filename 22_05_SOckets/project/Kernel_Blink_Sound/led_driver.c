#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

#define GPIO_LED 18 // GPIO18 for LED
static struct timer_list blink_timer;
static int blink_period_ms = 1000; // Default 1s period
static int blink_duration_ms = 0;
static int blink_count = 0;

static struct kobject *led_kobj;

// Toggle LED state
static void blink_timer_callback(struct timer_list *t) {
    static int state = 0;
    gpio_set_value(GPIO_LED, state);
    state = !state;

    if (--blink_count > 0) {
        mod_timer(&blink_timer, jiffies + msecs_to_jiffies(blink_period_ms / 2));
    } else {
        gpio_set_value(GPIO_LED, 0); // Turn off LED
    }
}

// Exported function to trigger blinking
void trigger_led_blink(int duration_ms) {
    blink_duration_ms = duration_ms;
    blink_count = duration_ms / (blink_period_ms / 2);
    gpio_set_value(GPIO_LED, 1); // Start with LED on
    mod_timer(&blink_timer, jiffies + msecs_to_jiffies(blink_period_ms / 2));
}
EXPORT_SYMBOL(trigger_led_blink);

// Sysfs attribute for period
static ssize_t period_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", blink_period_ms);
}

static ssize_t period_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret = kstrtoint(buf, 10, &blink_period_ms);
    if (ret || blink_period_ms <= 0)
        return -EINVAL;
    return count;
}

static struct kobj_attribute period_attr = __ATTR(period, 0644, period_show, period_store);

static int __init led_driver_init(void) {
    int ret;

    // Request GPIO
    ret = gpio_request(GPIO_LED, "led");
    if (ret) {
        printk(KERN_ERR "GPIO request failed: %d\n", ret);
        return ret;
    }
    gpio_direction_output(GPIO_LED, 0);

    // Set up timer
    timer_setup(&blink_timer, blink_timer_callback, 0);

    // Create sysfs entry
    led_kobj = kobject_create_and_add("ledblink", kernel_kobj);
    if (!led_kobj) {
        gpio_free(GPIO_LED);
        return -ENOMEM;
    }
    ret = sysfs_create_file(led_kobj, &period_attr.attr);
    if (ret) {
        kobject_put(led_kobj);
        gpio_free(GPIO_LED);
        return ret;
    }

    printk(KERN_INFO "LED driver initialized\n");
    return 0;
}

static void __exit led_driver_exit(void) {
    del_timer_sync(&blink_timer);
    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);
    if (led_kobj)
        kobject_put(led_kobj);
    printk(KERN_INFO "LED driver exited\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom LED driver for blinking");