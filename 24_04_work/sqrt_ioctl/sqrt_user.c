#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/IOCTL_SQRT"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)

struct int_data {
    double input;
    double result;
};

int main() {
    int fd;
    struct int_data data;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Get user input
    printf("Enter input value :  ");
    scanf("%ld", &data.input);

    // Send data to kernel
    ioctl(fd, IOCTL_SET_DATA, &data);

    // Get result from kernel
    ioctl(fd, IOCTL_GET_RESULT, &data.result);

    printf("Result: %ld\n", data.result);

    close(fd);
    return 0;
}

