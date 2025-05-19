#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/pwm.h>
#include <linux/of.h>

#define PWM_PERIOD_NS 1000000 // 1ms period = 1kHz

static struct pwm_device *pwm = NULL;

static int pwm_set_brightness(int level)
{
    u32 duty_cycle;

    switch (level) {
    case 0:
        duty_cycle = 0;
        break;
    case 1:
        duty_cycle = PWM_PERIOD_NS;
        break;
    case 2:
        duty_cycle = PWM_PERIOD_NS * 0.25;
        break;
    case 3:
        duty_cycle = PWM_PERIOD_NS * 0.5;
        break;
    case 4:
        duty_cycle = PWM_PERIOD_NS * 0.75;
        break;
    case 5:
        duty_cycle = PWM_PERIOD_NS * 0.9;
        break;
    default:
        duty_cycle = 0;
        break;
    }

    pwm_config(pwm, duty_cycle, PWM_PERIOD_NS);
    return pwm_enable(pwm);
}

static int uart_listener_thread(void *data)
{
    struct file *uart_file;
    mm_segment_t oldfs;
    char ch;

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    uart_file = filp_open("/dev/serial0", O_RDONLY, 0);
    if (IS_ERR(uart_file)) {
        pr_err("Failed to open /dev/serial0\n");
        set_fs(oldfs);
        return -1;
    }

    pr_info("Listening on UART...\n");

    while (!kthread_should_stop()) {
        if (kernel_read(uart_file, &ch, 1, &uart_file->f_pos) > 0) {
            pr_info("Received: %c\n", ch);
            if (ch >= '0' && ch <= '5') {
                pwm_set_brightness(ch - '0');
            }
        }
        msleep(100);
    }

    filp_close(uart_file, NULL);
    set_fs(oldfs);
    return 0;
}

static struct task_struct *listener_task;

static int __init uart_led_init(void)
{
    struct device_node *np = of_find_node_by_path("/soc/pwm@7e20c000"); // pwm0
    if (!np) {
        pr_err("PWM node not found\n");
        return -ENODEV;
    }

    pwm = of_pwm_get(np, NULL);
    if (IS_ERR(pwm)) {
        pr_err("Failed to get PWM device\n");
        return PTR_ERR(pwm);
    }

    pwm_config(pwm, 0, PWM_PERIOD_NS);
    pwm_enable(pwm);

    listener_task = kthread_run(uart_listener_thread, NULL, "uart_listener");
    if (IS_ERR(listener_task)) {
        pr_err("Failed to create thread\n");
        pwm_disable(pwm);
        return PTR_ERR(listener_task);
    }

    pr_info("UART LED module loaded\n");
    return 0;
}

static void __exit uart_led_exit(void)
{
    if (listener_task)
        kthread_stop(listener_task);

    if (pwm) {
        pwm_config(pwm, 0, PWM_PERIOD_NS);
        pwm_disable(pwm);
        pwm_put(pwm);
    }

    pr_info("UART LED module unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash");
MODULE_DESCRIPTION("UART to PWM LED control on RPi");

module_init(uart_led_init);
module_exit(uart_led_exit);

