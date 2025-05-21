#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEVICE_PATH "/dev/waitqueue_dev"

int main()
{
    int fd, value = 1, ret;

    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return errno;
    }

    printf("Writing to device to set flag...\n");
    ret = write(fd, &value, sizeof(int));
    if (ret < 0) {
        perror("Failed to write to device");
        close(fd);
        return errno;
    }

    printf("Write successful\n");

    close(fd);
    return 0;
}