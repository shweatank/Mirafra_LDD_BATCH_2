#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>

#define GPIO_BASE_PHYS  0xfE200000  // BCM2711 GPIO base (Linux physical address)
#define GPIO_SIZE       0xB4        // Size of GPIO registers (enough to map all registers)

static void __iomem *gpio_base = NULL;

// Register offsets
#define GPFSEL1     0x04
#define GPSET0      0x1C
#define GPCLR0      0x28

#define GPIO_PIN    18  // GPIO pin number for LED

static int __init gpio_led_init(void)
{
    uint32_t val;

    pr_info("GPIO LED driver init\n");

    // Map physical GPIO base to kernel virtual address
    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);
    if (!gpio_base) {
        pr_err("Failed to map GPIO base\n");
        return -ENOMEM;
    }

    // Set GPIO18 as output: GPFSEL1 bits [24:26] = 001
    val = readl(gpio_base + GPFSEL1);
    val &= ~(0x7 << 24); // Clear bits
    val |=  (0x1 << 24); // Set output
    writel(val, gpio_base + GPFSEL1);

    // Blink LED 5 times
    for (int i = 0; i < 5; i++) {
        // Set GPIO18 (LED ON)
        writel(1 << GPIO_PIN, gpio_base + GPSET0);
        msleep(500);

        // Clear GPIO18 (LED OFF)
        writel(1 << GPIO_PIN, gpio_base + GPCLR0);
        msleep(500);
    }

    return 0;
}

static void __exit gpio_led_exit(void)
{
    // Turn off LED
    writel(1 << GPIO_PIN, gpio_base + GPCLR0);

    // Unmap GPIO memory
    if (gpio_base)
        iounmap(gpio_base);

    pr_info("GPIO LED driver exit\n");
}

module_init(gpio_led_init);
module_exit(gpio_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant @ TechDhaba");
MODULE_DESCRIPTION("BCM2711 GPIO LED Blink using ioremap");
