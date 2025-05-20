#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/kernel_int"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, int *) //writting to kernel sapce
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)          //reading from kernel space

int num1;
int result;

int main() {
    int fd, result;
    //struct calc_data calc;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Get user input
    printf("Enter number: ");
    scanf("%d", &num1);

    // Send data to kernel
    ioctl(fd, IOCTL_SET_DATA, &num1);

    // Get result from kernel
    ioctl(fd, IOCTL_GET_RESULT, &result);

    printf("Result: %d\n", result);

    close(fd);
    return 0;
}

