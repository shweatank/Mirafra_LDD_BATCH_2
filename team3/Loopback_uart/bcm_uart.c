#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>


#define UART_DEV "/dev/ttyAMA0"
#define LED_GPIO 529
#define TX_MSG "Hello from Kernel via UART\n"


static struct task_struct *uart_task;

static int uart_thread_fn(void *data)

{
    struct file *filp;
    
    char rx_buf[64] = {0};
    loff_t pos = 0;
    int ret;
    int msg_len=strlen(TX_MSG);

    pr_info("uart Tx thread started..\n");

    // Open UART device
    filp = filp_open(UART_DEV, O_RDWR | O_NOCTTY | O_SYNC, 0);
    if (IS_ERR(filp)) {
        pr_err("Failed to open %s\n", UART_DEV);
        return PTR_ERR(filp);
    }
    // Write data to UART
    ret = kernel_write(filp,TX_MSG,msg_len, &pos);
    pr_info("wrote %d bytes to UART\n",ret);

    // Small delay
    msleep(00);

    // Try to read data from UART
    pos = 0;
    ret = kernel_read(filp, rx_buf,sizeof(rx_buf)-1, &pos);
    if (ret > 0) {
	    rx_buf[ret]='\0';
	    pr_info("received from UART : %s and ret = %d \n",rx_buf,ret);
        // Blink LED if data is received
	pr_info("Blinking LED now...\n");
        gpio_set_value(LED_GPIO, 1);
        msleep(1000);
        gpio_set_value(LED_GPIO, 0);
	pr_info("LED should have blinked\n");
    } else {
        pr_info("No data received\n");
    }
    filp_close(filp, NULL);
    return 0;
}

static int __init uart_led_init(void)
{

    int ret;

    pr_info("UART LED module loading...\n");

    ret = gpio_request(LED_GPIO, "LED_GPIO");
    if (ret) {
        pr_err("Failed to request GPIO\n");
        return ret;
    }
    ret=gpio_direction_output(LED_GPIO, 0);
    if(ret)
    {
	    pr_err("failed to request GPIO\n");
	    return ret;
    }

    uart_task = kthread_run(uart_thread_fn, NULL, "uart_loop_thread");
    if (IS_ERR(uart_task)) {
        gpio_free(LED_GPIO);
        pr_err("Failed to start UART thread\n");
        return PTR_ERR(uart_task);
    }

    return 0;
}

static void __exit uart_led_exit(void)
{

    gpio_set_value(LED_GPIO, 0);
    gpio_free(LED_GPIO);
    pr_info("UART LED module unloaded\n");
}

module_init(uart_led_init);
module_exit(uart_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("team");
MODULE_DESCRIPTION("UART TX with LED blink kernel module using /dev/ttyAMA0");
