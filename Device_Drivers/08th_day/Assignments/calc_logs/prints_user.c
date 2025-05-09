#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

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
    struct calc_data input_data;
    int result;

    // Open the device
    fd = open("/dev/all_logs", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Get user input
    printf("Enter operation (e.g., 5 + 3): ");
    scanf("%d %c %d", &input_data.num1, &input_data.operation, &input_data.num2);

    // Send data to kernel (using IOCTL)
    if (ioctl(fd, IOCTL_SET_DATA, &input_data) < 0) {
        perror("IOCTL_SET_DATA failed");
        close(fd);
        return 1;
    }

    // Get result from kernel (using IOCTL)
    if (ioctl(fd, IOCTL_GET_RESULT, &result) < 0) {
        perror("IOCTL_GET_RESULT failed");
        close(fd);
        return 1;
    }

    // Output the result
    printf("Result: %d\n", result);

    // Close the device
    close(fd);
    return 0;
}

