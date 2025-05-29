#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define UART_DEVICE "/dev/ttyUSB0"

int main() {
    int uart_fd = open(UART_DEVICE, O_WRONLY | O_NOCTTY);
    if (uart_fd < 0) {
        perror("Unable to open UART device");
        return 1;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag &= ~PARENB;       // No parity
    options.c_cflag &= ~CSTOPB;       // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;           // 8 data bits
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CRTSCTS;      // No flow control

    options.c_lflag = 0;              // No canonical mode
    options.c_oflag = 0;
    options.c_iflag = 0;

    tcsetattr(uart_fd, TCSANOW, &options);

    // Message to send
    const char *message = "Hello from UART!\n";

    int bytes_written = write(uart_fd, message, strlen(message));
    if (bytes_written < 0) {
        perror("Failed to write to UART");
    } else {
        printf("Sent %d bytes: %s\n", bytes_written, message);
    }

    close(uart_fd);
    return 0;
}
