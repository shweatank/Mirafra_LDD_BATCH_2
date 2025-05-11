#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/sys_control"
#define BUFFER_SIZE 256

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Open the device
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Clear terminal and set up for feedback
    printf("\033[2J\033[H");

    while (1) {
        // Read feedback from device
        bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0) {
            perror("Failed to read device");
            break;
        }
        buffer[bytes_read] = '\0';

        // Print with green color
        printf("\033[H\033[32m%s\033[0m", buffer);
        fflush(stdout);

        // Control frame rate (10 FPS)
        usleep(100000);
    }

    // Close the device
    close(fd);
    return 0;
}
