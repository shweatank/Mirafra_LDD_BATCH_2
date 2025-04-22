#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

int main()
{
    char *fname = "example.txt";
    int fd = open(fname,O_WRONLY);
    if(fd == -1) {
        perror("error");
        return 0;
    }
    char data[] = "shubham katole";
    write(fd,data,strlen(data));
    close(fd);


    // for reading data
    fd = open(fname,O_RDONLY);
    if(fd == -1) {
        perror("error");
        return 0;
    }

    char buffer[100];
    size_t bytes_read = read(fd, buffer, sizeof(buffer));
    buffer[bytes_read] = '\0';
    printf("buffer is %s\n",buffer);
    close(fd);


    return 0;
}
