#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MAX_SIZE 100

int main() {
int fd1, fd2, fd_out;
char buffer[MAX_SIZE];
int num1, num2;
ssize_t bytes;

fd1 = open("data1.txt", O_RDONLY);
if (fd1 < 0) {
    printf("cant open data1.txt: %s\n", strerror(errno));
return 1;
}

bytes = read(fd1, buffer, MAX_SIZE - 1);
if (bytes < 0) {
    printf("error reading data1.txt: %s\n", strerror(errno));
    close(fd1);
return 1;
}
buffer[bytes] = '\0';
num1 = atoi(buffer);
close(fd1);

fd2 = open("data2.txt", O_RDONLY);
if (fd2 < 0) {
   printf("cant open data2.txt: %s\n", strerror(errno));
return 1;
}

bytes = read(fd2, buffer, MAX_SIZE - 1);
if (bytes < 0) {
    printf("error reading data2.txt: %s\n", strerror(errno));
    close(fd2);
return 1;
}
buffer[bytes] = '\0';
num2 = atoi(buffer);
close(fd2);

int sum = num1 + num2;
int diff = num1 - num2;

fd_out = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (fd_out < 0) {
    printf("cant open output.txt: %s\n", strerror(errno));
return 1;
}

snprintf(buffer, MAX_SIZE, "input: %d, %d\nsum: %d\ndiff: %d\n",num1, num2, sum, diff);
if (write(fd_out, buffer, strlen(buffer)) < 0) {
    printf("error writing to output.txt: %s\n", strerror(errno));
    close(fd_out);
return 1;
}

close(fd_out);
printf("result in output.txt\n");
return 0;
}
