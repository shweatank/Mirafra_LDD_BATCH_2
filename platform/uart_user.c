//final user space code
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SERIAL_PORT "/dev/rpi_uart"

int main() {
    int fd;
    char buf[256];
    ssize_t n;
    const char *init_msg = "Hello from Raspi\n";

    // Open serial port
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        perror("Failed to open serial port");
        exit(1);
    }

    // Set non-blocking read
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) 
    {
        perror("Failed to set non-blocking");
        close(fd);
        exit(1);
    }

    printf("Serial port opened. Sending initial message...\n");

    // Send initial message
    n = write(fd, init_msg, strlen(init_msg)); //count 
    if (n < 0) {
        perror("Write error");
        close(fd);
        exit(1);
    }
    printf("Sent: %s", init_msg);

    // Main loop
    while (1) {
        // Read from serial port
        n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Received: %s", buf);  // Just display whatever was received
            // LED will blink in kernel on each byte received
        } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) // It's OK, no data available now — try again later
         {
            perror("Read error");
            close(fd);
            exit(1);
        }

        // Send periodic message every 5 seconds
        sleep(5);
        n = write(fd, init_msg, strlen(init_msg));
        if (n < 0) {
            perror("Write error");
            close(fd);
            exit(1);
        }
        printf("Sent: %s", init_msg);
    }
}
/*linux kernel will send hello from raspeerypi through uart to the uart hardware on  rasperrypi board, so that upon receiving it will blibk an led for every  1 byte of data  and gives received hello from rasperrypi through the same uart & message will be sent every 5 sec"*/
