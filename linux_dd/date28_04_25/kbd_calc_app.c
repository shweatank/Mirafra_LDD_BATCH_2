#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/simple_calculator"
#define IOCTL_SET_DATA _IOW(100, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(100, 1, int *)

struct calc_data {
    int num1;
    int num2;
    char operation;
    int result;
};

int main() {
    int fd;
    struct calc_data data;
    int result;

    // Open the device file
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    // Set data for the calculator
    data.num1 = 10;
    data.num2 = 5;
    data.operation = '0';  // Operation code (0: addition)
    
    if (ioctl(fd, IOCTL_SET_DATA, &data) < 0) {
        perror("Failed to set data");
        close(fd);
        return -1;
    }

    // Get the result
    if (ioctl(fd, IOCTL_GET_RESULT, &result) < 0) {
        perror("Failed to get result");
        close(fd);
        return -1;
    }

    printf("Result: %d\n", result);

    // Close the device file
    close(fd);
    return 0;
}

