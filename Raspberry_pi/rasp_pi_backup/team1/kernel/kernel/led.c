#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/file.h>
#include <linux/slab.h>

#define GPIO_LED 17
#define UART_PATH "/dev/serial0"

static struct task_struct *uart_thread;

static int uart_listener(void *data) {
    struct file *uart_filp;
    char buf[2] = {0};
    loff_t pos = 0;
    int ret;

    uart_filp = filp_open(UART_PATH, O_RDONLY | O_NONBLOCK, 0);
    if (IS_ERR(uart_filp)) {
        pr_err("Failed to open %s\n", UART_PATH);
        return PTR_ERR(uart_filp);
    }

    pr_info("UART listener thread started\n");

    while (!kthread_should_stop()) {
        ret = kernel_read(uart_filp, buf, 1, &pos);
        if (ret > 0) {
            pr_info("UART Received: %c\n", buf[0]);

            switch (buf[0]) {
                case '0':
                    gpio_set_value(GPIO_LED, 0);
                    break;
                case '1':
                    gpio_set_value(GPIO_LED, 1);
                    break;
                default:
                    pr_info("Invalid command received\n");
                    break;
            }
        }
        msleep(100);
    }

    filp_close(uart_filp, NULL);
    return 0;
}

static int __init led_uart_init(void) {
    int ret;

    ret = gpio_request(GPIO_LED, "led_gpio");
    if (ret) {
        pr_err("Failed to request GPIO %d\n", GPIO_LED);
        return ret;
    }

    gpio_direction_output(GPIO_LED, 0);
    pr_info("LED GPIO %d initialized\n", GPIO_LED);

    uart_thread = kthread_run(uart_listener, NULL, "uart_listener_thread");
    if (IS_ERR(uart_thread)) {
        pr_err("Failed to create UART thread\n");
        gpio_free(GPIO_LED);
        return PTR_ERR(uart_thread);
    }

    return 0;
}

static void __exit led_uart_exit(void) {
    if (uart_thread)
        kthread_stop(uart_thread);

    gpio_set_value(GPIO_LED, 0);
    gpio_free(GPIO_LED);
    pr_info("LED UART module unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash R");
MODULE_DESCRIPTION("UART to GPIO LED Control Kernel Module");

module_init(led_uart_init);
module_exit(led_uart_exit);
