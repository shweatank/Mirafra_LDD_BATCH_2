#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define DEVICE_FILE "/dev/waitqueue_dev"

int main() {
    int fd;
    char buffer[100];

    // Open the device
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open the device");
        return -1;
    }

    // Read from the device (this will block until the condition flag is set to 1)
    printf("Attempting to read from device...\n");
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == 0) {
        printf("Successfully read from device.\n");
    } else {
        printf("Failed to read from device or timed out.\n");
    }

    // Write to the device (this will wake up the sleeping reader)
    printf("Writing to device...\n");
    ssize_t bytes_written = write(fd, "1", 1);  // Set condition_flag to 1
    if (bytes_written == -1) {
        perror("Failed to write to the device");
    } else {
        printf("Condition flag set, reader should wake up.\n");
    }

    // Close the device
    close(fd);
    return 0;
}

