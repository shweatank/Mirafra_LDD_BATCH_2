#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/IOCTL_STRING"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)

struct str_data {
    char input_str[100];
    char res_str[100];
};

int main() {
    int fd, result;
    struct str_data str;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Get user input
    printf("Enter input String :  ");
    scanf(" %[^\n]", str.input_str);

    // Send data to kernel
    ioctl(fd, IOCTL_SET_DATA, &str);

    // Get result from kernel
    ioctl(fd, IOCTL_GET_RESULT, &str.res_str);

    printf("Result: %s\n", str.res_str);

    close(fd);
    return 0;
}

