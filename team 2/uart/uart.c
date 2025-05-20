#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define DEVICE "/dev/ttyUSB0"  // Change if needed

int main() {
    int uart_fd;
    struct termios options;
    char send_buf[256];
    char recv_buf[256];

    // Open UART device
    uart_fd = open(DEVICE, O_RDWR | O_NOCTTY);
    if (uart_fd == -1) {
        perror("Unable to open UART");
        return 1;
    }

    // Configure UART
    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, B9600);  // Set baud rate
    cfsetospeed(&options, B9600);
    options.c_cflag &= ~PARENB;    // No parity
    options.c_cflag &= ~CSTOPB;    // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;        // 8 data bits
    options.c_cflag |= CREAD | CLOCAL;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    tcsetattr(uart_fd, TCSANOW, &options);

    printf("UART ready. Type messages to send. Type 'exit' to quit.\n");

    while (1) {
        printf("> ");
        fgets(send_buf, sizeof(send_buf), stdin);
        send_buf[strcspn(send_buf, "\n")] = 0; // Remove newline

        if (strcmp(send_buf, "exit") == 0)
            break;

        // Send to UART
        int sent = write(uart_fd, send_buf, strlen(send_buf));
        if (sent < 0) {
            perror("UART write failed");
            break;
        }

        // Read response (optional, if device echoes)
        int n = read(uart_fd, recv_buf, sizeof(recv_buf) - 1);
        if (n > 0) {
            recv_buf[n] = '\0';
            printf("Received: %s\n", recv_buf);
        }
    }

    close(uart_fd);
    return 0;
}
// uart.c
/*#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main() {
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("Unable to open UART");
        return 1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    tcsetattr(fd, TCSANOW, &options);

    char msg[] = "Hello UART from RPi!\n";
    write(fd, msg, sizeof(msg));

    close(fd);
    return 0;
}*/

