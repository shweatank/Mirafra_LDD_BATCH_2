// uart_hello.c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#define UART0_BASE 0xFE201000  // PL011 base for RPi 4
#define UART_FR    0x18        // Flag Register
#define UART_DR    0x00        // Data Register
#define UART_CR    0x30        // Control Register
#define UART_IBRD  0x24        // Integer Baud Rate
#define UART_FBRD  0x28        // Fractional Baud Rate
#define UART_LCRH  0x2C        // Line Control
#define UART_IMSC  0x38        // Interrupt Mask

static void __iomem *uart_base;

static void uart_send_char(char c) {
    // Wait until TX FIFO is not full
    while (readl(uart_base + UART_FR) & (1 << 5));
    writel(c, uart_base + UART_DR);
}

static void uart_send_string(const char *s) {
    while (*s) {
        uart_send_char(*s++);
    }
}

static int __init uart_init(void) {
    pr_info("UART Hello Driver Init\n");

    uart_base = ioremap(UART0_BASE, 0x100);
    if (!uart_base) {
        pr_err("UART ioremap failed\n");
        return -ENOMEM;
    }

    // Disable UART before configuring
    writel(0x0, uart_base + UART_CR);

    // Set baud rate (48MHz / (16 * 26.1875)) = 115200
    writel(26, uart_base + UART_IBRD);
    writel(3, uart_base + UART_FBRD);

    // 8N1, enable FIFO
    writel((3 << 5) | (1 << 4), uart_base + UART_LCRH);

    // Disable all UART interrupts
    writel(0x0, uart_base + UART_IMSC);

    // Enable UART, TX and RX
    writel((1 << 0) | (1 << 8) | (1 << 9), uart_base + UART_CR);

    // Send "Hello"
    uart_send_string("Hello\n");

    return 0;
}

static void __exit uart_exit(void) {
    writel(0x0, uart_base + UART_CR);  // Disable UART
    iounmap(uart_base);
    pr_info("UART Hello Driver Exit\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash R");
MODULE_DESCRIPTION("Minimal UART driver that sends Hello over GPIO14 TX");
