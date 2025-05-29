void main() {
    uart_init();
    uart_puts("Hello, UART!\n");

    while (1) {
        char c = uart_getc();
        uart_putc(c);  // Echo
    }
}

