#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/string_dri"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)
#define SIZE 100

struct calc_data {
    char str[SIZE];
};

int main() {
    int fd;
    struct calc_data calc;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Get user input
    printf("Enter the given string ");
    scanf("%[^\n]s", calc.str);

    // Send data to kernel
    ioctl(fd, IOCTL_SET_DATA, &calc);

    // Get result from kernel
    ioctl(fd, IOCTL_GET_RESULT, &calc);

    printf("Reversed string: %s\n", calc.str);

    close(fd);
    return 0;
}

