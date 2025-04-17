#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main() {
int fd;
char buffer[BUFFER_SIZE];
ssize_t bytes_read;
    
fd = open("data.txt", O_RDONLY);
if (fd == -1) {
   perror("Error opening file");
return 1;
}
    
while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
     buffer[bytes_read] = '\0'; 
     printf("%s", buffer);
}
    
if (bytes_read == -1) {
    perror("Error reading file");
    close(fd);
return 1;
}
    
if (close(fd) == -1) {
    perror("Error closing file");
return 1;
}
return 0;
}
