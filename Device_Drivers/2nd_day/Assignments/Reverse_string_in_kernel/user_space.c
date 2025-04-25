#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char input[100];
    char output[100];

    fd = open("/dev/charcalc", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter a string : ");
    fgets(input, sizeof(input), stdin);

    write(fd, input, strlen(input));

    read(fd, output, sizeof(output));
    printf("From kernel: %s\n", output);

    close(fd);
    return 0;
}
