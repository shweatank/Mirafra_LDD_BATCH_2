// uart_sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main() {
    int serial_port = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        perror("Failed to open serial port");
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_port, &tty) != 0) {
        perror("Error getting tty attributes");
        close(serial_port);
        return 1;
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_iflag = 0;
    tty.c_oflag = 0;

    tcflush(serial_port, TCIFLUSH);
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("Error setting tty attributes");
        close(serial_port);
        return 1;
    }

    while (1) {
        const char *msg = "Hello from UART sender!\n";
        int bytes_written = write(serial_port, msg, strlen(msg));
        if (bytes_written < 0) {
            perror("UART write failed");
            break;
        }
        printf("Sent: %s", msg);
        sleep(2);
    }

    close(serial_port);
    return 0;
}
