#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/uaccess.h>

#define UART_DEVICE "/dev/serial0"
#define MESSAGE "Hello from Raspberry Pi kernel!\n"

static int __init uart_module_init(void) {
    struct file *file=NULL;
    struct tty_struct *tty=NULL;

    pr_info("UART kernel module init\n");

    // Open the UART device
    file = filp_open(UART_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
    if (IS_ERR(file)) {
        pr_err("Unable to open UART device: %s\n", UART_DEVICE);
        return PTR_ERR(file);
    }

    // Ensure file is a TTY
    if (!file->private_data) {
        pr_err("Not a TTY device: %s\n", UART_DEVICE);
        filp_close(file, NULL);
        return -ENODEV;
    }

    tty = (struct tty_struct *)file->private_data;

    if (!tty || !tty->ops || !tty->ops->write || !tty->ops->flush_buffer) {
        pr_err("Invalid TTY or missing ops\n");
        filp_close(file, NULL);
        return -ENODEV;
    }

    // Write the message to UART
    tty->ops->write(tty, MESSAGE, strlen(MESSAGE));
    tty->ops->flush_buffer(tty);
    pr_info("UART message sent\n");

    filp_close(file, NULL);
    return 0;
}

static void __exit uart_module_exit(void) {
    pr_info("UART kernel module exit\n");
}

module_init(uart_module_init);
module_exit(uart_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("UART Kernel Module for Raspberry Pi 4");

