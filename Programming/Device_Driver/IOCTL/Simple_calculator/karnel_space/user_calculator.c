#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/simple_calculator"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)

struct calc_data {
    int num1;
    int num2;
    char operation;
    int result;
};

int main() {
    int fd, result;
    struct calc_data calc;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Get user input
    printf("Enter calculation (e.g., 10 + 5): ");
    scanf("%d %c %d", &calc.num1, &calc.operation, &calc.num2);

    // Send data to kernel
    ioctl(fd, IOCTL_SET_DATA, &calc);

    // Get result from kernel
    ioctl(fd, IOCTL_GET_RESULT, &result);

    printf("Result: %d\n", result);

    close(fd);
    return 0;
}

