#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main() {
    const char *port = "/dev/serial0";  // UART0 on Raspberry Pi
    int serial_port = open(port, O_WRONLY | O_NOCTTY);
    if (serial_port < 0) {
        perror("Error opening serial port on Pi");
        return 1;
    }

    struct termios tty;
    tcgetattr(serial_port, &tty);

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_iflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    tcsetattr(serial_port, TCSANOW, &tty);

    char message[256];
    printf("Enter message to send: ");
    fgets(message, sizeof(message), stdin);

    write(serial_port, message, strlen(message));
    printf("Sent: %s", message);

    close(serial_port);
    return 0;
}
