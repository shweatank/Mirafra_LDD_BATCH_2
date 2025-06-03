#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int serial_port = open("/dev/rpiuart", O_WRONLY);
    if (serial_port < 0) {
        perror("Failed to open serial port");
        return 1;
    }

    while (1) {
        const char *msg = "Hello from UART sender!\n";
        int bytes_written = write(serial_port, msg, strlen(msg));
        if (bytes_written < 0) {
            perror("UART write failed");
            close(serial_port);
            return 1;
        }
        printf("Sent: %s", msg);
        sleep(2);
    }

    close(serial_port);
    return 0;
}