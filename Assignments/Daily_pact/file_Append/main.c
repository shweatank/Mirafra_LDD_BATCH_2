#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MAX_INPUT 256

int main() {
int fd;
char input[MAX_INPUT];
    
printf("Enter text to append: ");
if (fgets(input, MAX_INPUT, stdin) == NULL) {
    perror("Error reading input");
return 1;
}
    
fd = open("log.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
if (fd == -1) {
    perror("Error opening file");
return 1;
}
    
if (write(fd, input, strlen(input)) == -1) {
    perror("Error writing to file");
    close(fd);
return 1;
}
    
close(fd);
printf("Data appended to log.txt.\n");
return 0;
}
