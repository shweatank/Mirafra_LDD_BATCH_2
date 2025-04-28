#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd_dev, fd_log;
    char buf[200];
    ssize_t bytes_read;

    // Write to the device
    fd_dev = open("/dev/mychar", O_WRONLY);
    if (fd_dev < 0) {
        perror("Failed to open device for writing");
        return 1;
    }
    write(fd_dev, "10 9", strlen("10 9"));
    close(fd_dev);

    // Read from the device
    fd_dev = open("/dev/mychar", O_RDONLY);
    if (fd_dev < 0) {
        perror("Failed to open device for reading");
        return 1;
    }
    bytes_read = read(fd_dev, buf, sizeof(buf) - 1);
    if (bytes_read < 0) {
        perror("Failed to read from device");
        close(fd_dev);
        return 1;
    }
    buf[bytes_read] = '\0'; // Null-terminate
    close(fd_dev);

    // Save output to log.txt
    fd_log = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd_log < 0) {
        perror("Failed to open log file");
        return 1;
    }
    dprintf(fd_log, "From kernel: %s\n", buf);
    close(fd_log);

    printf("Kernel response saved in log.txt\n");

    return 0;
}

