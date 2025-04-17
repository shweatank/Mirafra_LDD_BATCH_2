#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main() {
int fd;
char *data = "Hello";
    
fd = open("example.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
if (fd == -1) {
    perror("Error opening file");
return 1;
}
    
if (write(fd, data, strlen(data)) == -1) {
    perror("Error writing to file");
    close(fd);
return 1;
}
    
if (close(fd) == -1) {
    perror("Error closing file");
return 1;
}
    
printf("Data written to example.txt successfully.\n");
return 0;
}
