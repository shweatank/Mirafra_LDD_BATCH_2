#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int serial_port = open("/dev/rpiuart", O_RDONLY);
    if (serial_port < 0) {
        perror("Failed to open serial port");
        return 1;
    }

    char buffer[256];
    while (1) {
        int bytes_read = read(serial_port, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Received: %s", buffer);
        } else if (bytes_read < 0) {
            perror("Read error");
            close(serial_port);
            return 1;
        }
    }

    close(serial_port);
    return 0;
}