// raspi_uart_to_led.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main() {
    int uart_fd = open("/dev/serial0", O_RDONLY | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Failed to open /dev/serial0");
        return 1;
    }

    int dev_fd = open("/dev/led_uart", O_WRONLY);
    if (dev_fd < 0) {
        perror("Failed to open /dev/led_uart");
        return 1;
    }

    // UART config
    struct termios options;
    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= CREAD | CLOCAL;
    tcsetattr(uart_fd, TCSANOW, &options);

    char ch;
    while (1) {
        int n = read(uart_fd, &ch, 1);
        if (n > 0) {
            write(dev_fd, &ch, 1);
            printf("Forwarded: %c\n", ch);
        }
    }

    close(uart_fd);
    close(dev_fd);
    return 0;
}
