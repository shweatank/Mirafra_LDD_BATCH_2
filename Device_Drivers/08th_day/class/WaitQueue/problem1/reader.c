#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEVICE_PATH "/dev/waitqueue_dev"

int main()
{
    int fd, value, ret;

    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return errno;
    }

    printf("Reading from device (will block until flag is set)...\n");
    ret = read(fd, &value, sizeof(int));
    if (ret < 0) {
        perror("Failed to read from device");
        close(fd);
        return errno;
    }

    printf("Read value: %d\n", value);

    close(fd);
    return 0;
}