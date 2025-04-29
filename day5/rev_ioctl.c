#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/string_rev"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM, 0, char *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, char *)


int main() {
    int fd, result;
	char input[20]="hello";
	char output[100];
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Get user input
	
    // Send data to kernel
    ioctl(fd, IOCTL_SET_DATA, &input);

    // Get result from kernel
    ioctl(fd, IOCTL_GET_RESULT, &output);

    printf("Result: %s\n", output);

    close(fd);
    return 0;
}

