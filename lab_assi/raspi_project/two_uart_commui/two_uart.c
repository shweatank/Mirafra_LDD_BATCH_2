#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>

#define SERIAL_PORT "/dev/serial0"
#define BAUDRATE B9600

int serial_port;

void configure_serial_port() {
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(serial_port, &tty) != 0) {
        perror("tcgetattr");
        exit(1);
    }

    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);    // Enable receiver, set local mode
    tty.c_cflag &= ~(PARENB | PARODD);  // No parity
    tty.c_cflag &= ~CSTOPB;             // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;            // No flow control

    tty.c_iflag = 0;
    tty.c_lflag = 0;
    tty.c_oflag = 0;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        exit(1);
    }
}

void* read_thread(void* arg) {
    char buf[256];
    while (1) {
        int n = read(serial_port, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Received: %s", buf);
            fflush(stdout);
        }
    }
    return NULL;
}

void* write_thread(void* arg) {
    char msg[256];
    while (1) {
        printf("Enter message: ");
        if (fgets(msg, sizeof(msg), stdin) != NULL) {
            write(serial_port, msg, strlen(msg));
        }
    }
    return NULL;
}

int main() {
    serial_port = open(SERIAL_PORT, O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        perror("open");
        return 1;
    }

    configure_serial_port();

    pthread_t reader, writer;
    pthread_create(&reader, NULL, read_thread, NULL);
    pthread_create(&writer, NULL, write_thread, NULL);

    pthread_join(reader, NULL);
    pthread_join(writer, NULL);

    close(serial_port);
    return 0;
}
