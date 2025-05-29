#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/uaccess.h>

#define UART_DEVICE "/dev/serial0"
#define MESSAGE "Hello from kernel!\n"

static int __init uart_module_init(void) {
    struct file *file;
    struct tty_struct *tty;

    pr_info("UART module loaded\n");

    // Open UART device
    file = filp_open(UART_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
    if (IS_ERR(file)) {
        pr_err("Failed to open UART device: %ld\n", PTR_ERR(file));
        return PTR_ERR(file);
    }

    // Get tty_struct from file
    tty = file->private_data;
    if (!tty || !tty->ops || !tty->ops->write) {
        pr_err("Invalid TTY structure or no write ops\n");
        filp_close(file, NULL);
        return -ENODEV;
    }

    // Write to UART
    tty->ops->write(tty, MESSAGE, strlen(MESSAGE));
    tty->ops->flush_buffer(tty);

    filp_close(file, NULL);
    return 0;
}

static void __exit uart_module_exit(void) {
    pr_info("UART module unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("UART kernel module example");

module_init(uart_module_init);
module_exit(uart_module_exit);

