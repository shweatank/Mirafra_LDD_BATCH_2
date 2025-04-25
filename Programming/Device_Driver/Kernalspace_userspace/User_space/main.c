#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char buf[200];

    fd = open("/dev/mychar", O_WRONLY);
    write(fd, "10 9", strlen("10 9"));
    close(fd);

    fd = open("/dev/mychar", O_RDONLY);
    read(fd, buf, sizeof(buf));
    printf("From kernel: %s\n", buf);
    close(fd);

    return 0;
}

