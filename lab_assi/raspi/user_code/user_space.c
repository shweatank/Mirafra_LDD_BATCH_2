#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define UART_DEVICE "/dev/serial0"  // or "/dev/ttyAMA0"
#define BAUD_RATE B115200

int main() {
    int uart_fd;
    struct termios options;
    char message[] = "Hello from user space UART!\n";

    // Open the UART device file
    uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        perror("Unable to open UART");
        return 1;
    }

    // Configure UART
    tcgetattr(uart_fd, &options);
    options.c_cflag = BAUD_RATE | CS8 | CLOCAL | CREAD; // 8-bit, enable receiver
    options.c_iflag = IGNPAR;  // Ignore parity errors
    options.c_oflag = 0;
    options.c_lflag = 0;       // Non-canonical mode

    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    // Write to UART
    int count = write(uart_fd, message, strlen(message));
    if (count < 0) {
        perror("UART TX error");
        close(uart_fd);
        return 1;
    }

    printf("Sent: %s", message);

    // Optional: Read response
    char buf[100];
    int rx_len = read(uart_fd, buf, sizeof(buf) - 1);
    if (rx_len > 0) {
        buf[rx_len] = '\0';
        printf("Received: %s\n", buf);
    }

    // Close UART
    close(uart_fd);
    return 0;
}

