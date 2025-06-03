#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define UART_DEVICE "/dev/ttyS0"
#define BAUDRATE B9600

int main(void)
{
    int fd;
    struct termios tty;
    const char *message = "Trigger\n";
    
    // Open UART device
    fd = open(UART_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Failed to open UART device");
        return -1;
    }

    // Configure UART
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);
    
    tty.c_cflag |= (CLOCAL | CREAD);    // Enable receiver, local mode
    tty.c_cflag &= ~CSIZE;              // Clear character size
    tty.c_cflag |= CS8;                 // 8 data bits
    tty.c_cflag &= ~PARENB;             // No parity
    tty.c_cflag &= ~CSTOPB;             // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;            // Disable hardware flow control
    
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw mode
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable software flow control
    tty.c_oflag &= ~OPOST;                          // Raw output
    
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Failed to set UART attributes");
        close(fd);
        return -1;
    }

    printf("Sending UART data to trigger GPIO interrupt...\n");
    
    // Send data periodically to simulate UART activity
    while (1) {
        int bytes_written = write(fd, message, strlen(message));
        if (bytes_written < 0) {
            perror("Failed to write to UART");
            break;
        }
        printf("Sent: %s", message);
        usleep(1000000); // Wait 1 second
    }

    close(fd);
    return 0;
}
