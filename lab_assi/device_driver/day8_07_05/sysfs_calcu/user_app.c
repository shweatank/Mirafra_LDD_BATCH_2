#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char input[100];
    char output[100];

    fd = open("/sys/kernel/my_sysfs/hello", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter operation (e.g., 5 + 3): ");
    fgets(input, sizeof(input), stdin);

    write(fd, input, strlen(input));
    lseek(fd, 0, SEEK_SET); 
    
    read(fd, output, sizeof(output));
    printf("From kernel: %s", output);

    close(fd);
    return 0;
}
