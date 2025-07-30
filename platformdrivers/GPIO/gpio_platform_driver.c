#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/slab.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of.h>

#define LED_OFF 0
#define LED_ON  1

/* 
 * Structure to hold LED-specific data
 * - GPIO descriptor
 * - LED class device structure
 */
struct drvled_data_st {
    struct gpio_desc *desc;           /* GPIO descriptor for LED control */
    struct led_classdev led_cdev;     /* LED subsystem class device */
};

static struct drvled_data_st *drvled_data;

/*
 * Helper function: Set LED ON/OFF using GPIO
 */
static void drvled_setled(unsigned int status)
{
    if (status == LED_ON)
        gpiod_set_value(drvled_data->desc, 1);  /* Set GPIO high to turn ON */
    else
        gpiod_set_value(drvled_data->desc, 0);  /* Set GPIO low to turn OFF */
}

/*
 * Callback for LED subsystem: changes LED brightness (ON/OFF)
 * brightness > 0 => LED ON, brightness = 0 => LED OFF
 */
static void drvled_change_state(struct led_classdev *led_cdev,
                                enum led_brightness brightness)
{
    if (brightness)
        drvled_setled(LED_ON);
    else
        drvled_setled(LED_OFF);
}

/*
 * Probe function: Called when device is matched with device tree entry
 */
static int drvled_probe(struct platform_device *pdev)
{
    struct device_node *child;

    /* Get the child node of the device tree node */
    child = of_get_next_child(pdev->dev.of_node, NULL);
    if (!child)
        return -ENODEV;

    /* Allocate memory for driver data */
    drvled_data = devm_kzalloc(&pdev->dev, sizeof(*drvled_data), GFP_KERNEL);
    if (!drvled_data)
        return -ENOMEM;

    /*
     * Get the GPIO descriptor for controlling LED
     * - "led" matches the gpio property name in the device tree
     * - GPIOD_OUT_LOW sets it as output and default low
     */
    drvled_data->desc = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(drvled_data->desc)) {
        dev_err(&pdev->dev, "Failed to get GPIO descriptor\n");
        return PTR_ERR(drvled_data->desc);
    }

    /* Configure LED class device parameters */
    drvled_data->led_cdev.name = of_get_property(child, "label", NULL); /* LED name from DT */
    drvled_data->led_cdev.brightness_set = drvled_change_state;          /* Brightness handler */

    /* Register with the LED subsystem */
    return devm_led_classdev_register(&pdev->dev, &drvled_data->led_cdev);
}

/*
 * Remove function: Called when the driver is removed/unloaded
 */
static int drvled_remove(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "LED driver removed.\n");
    return 0;
}

/*
 * Match table for Device Tree compatible string
 */
static const struct of_device_id of_drvled_match[] = {
    { .compatible = "led1,drvled" },   /* Match compatible property in DT */
    {},
};
MODULE_DEVICE_TABLE(of, of_drvled_match);

/*
 * Platform driver structure
 */
static struct platform_driver drvled_driver = {
    .driver = {
        .name   = "drvleds",               /* Name used for driver */
        .of_match_table = of_drvled_match, /* Match table */
    },
    .probe      = drvled_probe,            /* Probe callback */
    .remove     = drvled_remove,           /* Remove callback */
};

/* Register platform driver (handles module init and exit automatically) */
module_platform_driver(drvled_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO descriptor based LED driver");

