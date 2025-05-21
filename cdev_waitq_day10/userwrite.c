#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char input[100];

    fd = open("/dev/charcalc", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Enter operation (e.g.,hello): ");
    fgets(input, sizeof(input), stdin);

    write(fd, input, strlen(input));

    close(fd);
    return 0;
}

