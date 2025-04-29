#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/IOCTL_strrev"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, char *)

struct calc_data {
    char input_str[100];
    char res_str[100];
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
    printf("Enter String to be Reversed: ");
    fgets(calc.input_str, sizeof(calc.input_str), stdin);

    // Remove newline character
    size_t len = strlen(calc.input_str);
    if (len > 0 && calc.input_str[len - 1] == '\n') {
        calc.input_str[len - 1] = '\0';
    }

    // Send data to kernel
    if (ioctl(fd, IOCTL_SET_DATA, &calc) == -1) {
        perror("IOCTL_SET_DATA failed");
        close(fd);
        return EXIT_FAILURE;
    }

    // Get reversed string from kernel
    if (ioctl(fd, IOCTL_GET_RESULT, calc.res_str) == -1) {
        perror("IOCTL_GET_RESULT failed");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Reversed String from Kernel: %s\n", calc.res_str);

    close(fd);
    return 0;
}

