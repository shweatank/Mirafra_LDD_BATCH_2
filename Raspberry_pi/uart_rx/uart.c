#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main() {
    const char *port = "/dev/ttyUSB1";
    int serial_port = open(port, O_RDONLY | O_NOCTTY);
    if (serial_port < 0) {
        perror("Error opening /dev/ttyUSB0");
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

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    int total_bytes = 0;
    printf("Waiting for data...\n");

    while (1) {
        int n = read(serial_port, &buffer[total_bytes], sizeof(buffer) - total_bytes);
        if (n > 0) {
            total_bytes += n;
            if (buffer[total_bytes - 1] == '\n') {
                break;
            }
        } else {
            break;
        }
    }

    buffer[total_bytes] = '\0';
    printf("Received: %s\n", buffer);

    close(serial_port);
    return 0;
}
