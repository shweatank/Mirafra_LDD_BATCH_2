#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256

int main() {
    char src[MAX_FILENAME_LEN];
    char dest[MAX_FILENAME_LEN];

    write(STDOUT_FILENO, "Enter source file name: ", 25);
    ssize_t src_len = read(STDIN_FILENO, src, MAX_FILENAME_LEN);
    if (src_len <= 1) {
        write(STDERR_FILENO, "Invalid source filename\n", 24);
        return 1;
    }
    src[src_len - 1] = '\0';

    write(STDOUT_FILENO, "Enter destination file name: ", 29);
    ssize_t dest_len = read(STDIN_FILENO, dest, MAX_FILENAME_LEN);
    if (dest_len <= 1) {
        write(STDERR_FILENO, "Invalid destination filename\n", 29);
        return 1;
    }
    dest[dest_len - 1] = '\0';

    int src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("open source");
        return 1;
    }

    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("open destination");
        close(src_fd);
        return 1;
    }

    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, bytes_read) != bytes_read) {
            perror("write");
            close(src_fd);
            close(dest_fd);
            return 1;
        }
    }

    close(src_fd);
    close(dest_fd);

    write(STDOUT_FILENO, "File copied successfully!\n", 27);
    return 0;
}

