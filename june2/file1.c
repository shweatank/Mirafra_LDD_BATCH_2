#include <fcntl.h>    // for open()
#include <unistd.h>   // for close(), read(), write()
#include <stdio.h>    // for perror()
#include <string.h>   // for strlen()

int main() {
    int fd;
    const char *filename = "sample.txt";
    const char *message = "Hello\n File";

    // Step 1: Open file for writing (create if not exists, truncate if it does)
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        return 1;
    }

    // Step 2: Write message to file
    if (write(fd, message, strlen(message)) == -1) {
        perror("Error writing to file");
        close(fd);
        return 1;
    }

    close(fd);  // Step 3: Close the file

    // Step 4: Open file again for reading
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return 1;
    }

    // Step 5: Read content and print
    char buffer[100];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Error reading from file");
        close(fd);
        return 1;
    }

    buffer[bytesRead] = '\0';  // Null-terminate
    printf("File Content:\n%s\n", buffer);

    close(fd);
    return 0;
}

