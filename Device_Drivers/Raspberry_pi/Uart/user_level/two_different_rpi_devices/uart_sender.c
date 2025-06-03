#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int main() {
    int serial_port = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        perror("Error opening serial port");
        return 1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_port, &tty) != 0) {
        perror("Error from tcgetattr");
        return 1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    tty.c_iflag = 0;                                // No special input processing
    tty.c_lflag = 0;                                // No canonical mode
    tty.c_oflag = 0;                                // No remapping, no delays
    tty.c_cc[VMIN]  = 1;                            // Read at least 1 character
    tty.c_cc[VTIME] = 5;                            // 0.5 second read timeout

    tty.c_cflag |= (CLOCAL | CREAD);                // Enable receiver, local mode
    tty.c_cflag &= ~(PARENB | PARODD);              // No parity
    tty.c_cflag &= ~CSTOPB;                         // One stop bit
    tty.c_cflag &= ~CRTSCTS;                        // No flow control

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr");
        return 1;
    }

    while (1) {
        const char *msg = "Hello Raspberry Pi B!\n";
        write(serial_port, msg, strlen(msg));
        printf("Sent: %s", msg);
        sleep(2);
    }

    close(serial_port);
    return 0;
}
