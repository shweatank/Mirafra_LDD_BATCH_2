#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char input[100];
    char output[100];

    fd = open("/dev/keyboard_data", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter the two number a and b-->enter  and press keyboard key\n");
    fgets(input, sizeof(input), stdin);
    
    write(fd, input, strlen(input));
    
    sleep(2);

    read(fd, output, sizeof(output));
    
    printf("\nFrom kernel to user space: %s", output);

    close(fd);
    return 0;
}
