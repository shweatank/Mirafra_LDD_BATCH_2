#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char buff[1024];
    size_t ret;

    fd = open("/dev/keyboard_counter", O_RDONLY);
    if(fd < 0)
    {
        perror("failed to open device file.");
        return 1;
    }

    ret = read(fd, buff, sizeof(buff)-1);
    if(ret < 0)
    {
        perror("failed to read the data from file.");
        close(fd);
        return 1;
    }
    buff[ret] = '\0';
    printf("The pressed keys and their counts \n");
    printf("%s", buff); 

    close(fd);
    return 1;

}