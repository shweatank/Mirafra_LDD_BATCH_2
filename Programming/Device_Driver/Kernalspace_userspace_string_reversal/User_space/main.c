#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char write_buf[200], read_buf[200];

    printf("Enter a string: ");
    scanf("%199s", write_buf);

    fd = open("/dev/mychar", O_WRONLY);
    if (fd < 0) {
        perror("open write");
        return 1;
    }

    write(fd, write_buf, strlen(write_buf));
    close(fd);

    fd = open("/dev/mychar", O_RDONLY);
    if (fd < 0) {
        perror("open read");
        return 1;
    }

    read(fd, read_buf, sizeof(read_buf));
    printf("Reversed from kernel: %s\n", read_buf);
    close(fd);

    return 0;
}

