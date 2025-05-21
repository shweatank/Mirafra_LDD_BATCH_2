#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/kthread.h>

#define GPIO_BASE_PHYS  0xFE200000  // BCM2711 GPIO base address
#define GPIO_SIZE        0xB4

// GPIO Registers offsets
#define GPFSEL1          0x04
#define GPFSEL2          0x08
#define GPSET0           0x1C
#define GPCLR0           0x28
#define GPLEV0           0x34

#define GPIO_OUT         18   // Output LED pin
#define GPIO_IN          17   // Input pin

static void __iomem *gpio_base = NULL;
static struct task_struct *blinker_thread = NULL;
static bool running = true;

static int gpio_thread_fn(void *data)
{
    while (!kthread_should_stop()) {
        uint32_t input = readl(gpio_base + GPLEV0);

        if (input & (1 << GPIO_IN)) {
            // Input is HIGH - Blink LED
            writel(1 << GPIO_OUT, gpio_base + GPSET0);
            msleep(200);
            writel(1 << GPIO_OUT, gpio_base + GPCLR0);
            msleep(200);
        } else {
            // Input is LOW - Turn off LED
            writel(1 << GPIO_OUT, gpio_base + GPCLR0);
            msleep(200);
        }
    }
    return 0;
}

static int __init gpio_module_init(void)
{
    uint32_t val;

    pr_info("GPIO Input-Output Module Init\n");

    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_SIZE);
    if (!gpio_base) {
        pr_err("Failed to ioremap GPIO\n");
        return -ENOMEM;
    }

    // Configure GPIO17 (input) => GPFSEL1 bits 21-23 = 000
    val = readl(gpio_base + GPFSEL1);
    val &= ~(0x7 << 21);  // Clear bits
    writel(val, gpio_base + GPFSEL1);

    // Configure GPIO18 (output) => GPFSEL1 bits 24-26 = 001
    val = readl(gpio_base + GPFSEL1);
    val &= ~(0x7 << 24);  // Clear bits
    val |=  (0x1 << 24);  // Set as output
    writel(val, gpio_base + GPFSEL1);

    // Create kernel thread for GPIO monitoring
    blinker_thread = kthread_run(gpio_thread_fn, NULL, "gpio_blinker");
    if (IS_ERR(blinker_thread)) {
        pr_err("Failed to create thread\n");
        iounmap(gpio_base);
        return PTR_ERR(blinker_thread);
    }

    return 0;
}

static void __exit gpio_module_exit(void)
{
    pr_info("Exiting GPIO module\n");

    if (blinker_thread)
        kthread_stop(blinker_thread);

    writel(1 << GPIO_OUT, gpio_base + GPCLR0);  // Turn off LED

    if (gpio_base)
        iounmap(gpio_base);
}

module_init(gpio_module_init);
module_exit(gpio_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant @ TechDhaba");
MODULE_DESCRIPTION("BCM2711 GPIO17 input triggers GPIO18 LED output");
