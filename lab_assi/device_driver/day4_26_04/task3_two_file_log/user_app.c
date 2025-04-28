#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char buffer[100];
    fd = open("/dev/read_data", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    read(fd,buffer, sizeof(buffer));
    printf("From kernel: %s", buffer);

    close(fd);
    return 0;
}
