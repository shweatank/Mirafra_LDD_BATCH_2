#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define GPIO_LED 18        // GPIO pin for LED
#define GPIO_TRIGGER 17    // GPIO pin to simulate UART data trigger
#define DEVICE_NAME "gpio_interrupt"
#define CLASS_NAME "gpio_int_class"

static int major_number;
static struct class* gpio_class = NULL;
static struct device* gpio_device = NULL;
static int irq_number;
static int led_state = 0;

// Interrupt handler
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    led_state = !led_state; // Toggle LED state
    gpio_set_value(GPIO_LED, led_state);
    pr_info("GPIO Interrupt: LED %s\n", led_state ? "ON" : "OFF");
    return IRQ_HANDLED;
}

static int gpio_dev_open(struct inode *inode, struct file *file)
{
    pr_info("GPIO device opened\n");
    return 0;
}

static int gpio_dev_release(struct inode *inode, struct file *file)
{
    pr_info("GPIO device closed\n");
    return 0;
}

static struct file_operations fops = {
    .open = gpio_dev_open,
    .release = gpio_dev_release,
};

static int __init gpio_interrupt_init(void)
{
    int ret;

    // Register character device
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("Failed to register character device\n");
        return major_number;
    }

    // Create device class
    gpio_class = class_create(CLASS_NAME);
    if (IS_ERR(gpio_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        pr_err("Failed to create device class\n");
        return PTR_ERR(gpio_class);
    }

    // Create device
    gpio_device = device_create(gpio_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(gpio_device)) {
        class_destroy(gpio_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        pr_err("Failed to create device\n");
        return PTR_ERR(gpio_device);
    }

    // Request GPIOs
    ret = gpio_request(GPIO_LED, "LED GPIO");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", GPIO_LED);
        device_destroy(gpio_class, MKDEV(major_number, 0));
        class_destroy(gpio_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return ret;
    }

    ret = gpio_request(GPIO_TRIGGER, "Trigger GPIO");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", GPIO_TRIGGER);
        gpio_free(GPIO_LED);
        device_destroy(gpio_class, MKDEV(major_number, 0));
        class_destroy(gpio_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return ret;
    }

    // Set GPIO directions
    gpio_direction_output(GPIO_LED, 0);
    gpio_direction_input(GPIO_TRIGGER);

    // Request IRQ for GPIO_TRIGGER
    irq_number = gpio_to_irq(GPIO_TRIGGER);
    if (irq_number < 0) {
        pr_err("Failed to get IRQ for GPIO %d\n", GPIO_TRIGGER);
        gpio_free(GPIO_LED);
        gpio_free(GPIO_TRIGGER);
        device_destroy(gpio_class, MKDEV(major_number, 0));
        class_destroy(gpio_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return irq_number;
    }

    ret = request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING, "gpio_trigger", NULL);
    if (ret) {
        pr_err("Failed to request IRQ %d\n", irq_number);
        gpio_free(GPIO_LED);
        gpio_free(GPIO_TRIGGER);
        device_destroy(gpio_class, MKDEV(major_number, 0));
        class_destroy(gpio_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return ret;
    }

    pr_info("GPIO Interrupt Driver Loaded\n");
    return 0;
}

static void __exit gpio_interrupt_exit(void)
{
    free_irq(irq_number, NULL);
    gpio_free(GPIO_LED);
    gpio_free(GPIO_TRIGGER);
    device_destroy(gpio_class, MKDEV(major_number, 0));
    class_destroy(gpio_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("GPIO Interrupt Driver Unloaded\n");
}

module_init(gpio_interrupt_init);
module_exit(gpio_interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grok");
MODULE_DESCRIPTION("GPIO Interrupt Driver for LED Blinking");
