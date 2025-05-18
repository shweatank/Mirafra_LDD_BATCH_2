#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main() {
    int uart_fd = open("/dev/ttyUSB0", O_WRONLY | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Failed to open /dev/ttyUSB0");
        return 1;
    }

    // Configure UART
    struct termios options;
    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag &= ~PARENB; // no parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;     // 8 data bits
    options.c_cflag |= CREAD | CLOCAL;
    tcsetattr(uart_fd, TCSANOW, &options);

    char input;
    while (1) {
        printf("Enter brightness level (1-5), or q to quit: ");
        scanf(" %c", &input);

        if (input == 'q') break;

        if (input >= '1' && input <= '5') {
            write(uart_fd, &input, 1);
            printf("Sent: %c\n", input);
        } else {
            printf("Invalid input. Enter 1-5.\n");
        }
    }

    close(uart_fd);
    return 0;
}
