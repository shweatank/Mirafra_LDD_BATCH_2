#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char output[100];

    fd = open("/dev/charcalc", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    read(fd, output, sizeof(output));
    printf("From kernel: %s", output);

    close(fd);
    return 0;
}

