#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

int main(int argc, char *argv[])
{
    if(argc != 3){
        write(STDERR_FILENO, "Usage: ./file <filename> <message> \n", 36);
        return 1;
    }
    char *fname = argv[1];
    char *msg = argv[2];

    int fd = open(fname,O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(fd == -1) {
        perror("open");
        return 0;
    }

    if (write(fd,msg,strlen(msg)) == -1) {
        perror("file");
        close(fd);
        return 1;
    }
    close(fd);


    // for reading
    fd = open(fname,O_RDONLY);
    if(fd == -1) {
        perror("file");
        return 0;
    }


    char buffer[100];
    //size_t bytes_read = fread(buffer,1,sizeof(buffer),fptr);
    size_t bytes_read = read(fd, buffer, sizeof(buffer)-1);
    if(bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    buffer[bytes_read] = '\0';
    printf("File content : \n%s\n",buffer);
    close(fd);


    return 0;
}
