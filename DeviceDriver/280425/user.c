// user_read_counter.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
    int  fd;
    char buf[32];
    ssize_t n;

    /* 1) Open the device for reading */
    fd = open("/dev/keyboard_device", O_RDONLY);
    if (fd < 0) {
        perror("open(/dev/keyboard_device)");
        return EXIT_FAILURE;
    }

    /* 2) Read the counter (as text) */
    n = read(fd, buf, sizeof(buf)-1);
    if (n < 0) {
        perror("read()");
        close(fd);
        return EXIT_FAILURE;
    }
    buf[n] = '\0';

    /* 3) Print it */
    printf("Key-press counter = %s", buf);

    close(fd);
    return EXIT_SUCCESS;
}


//sudo mknod /dev/mychar c 235 0
//sudo chmod 666 /dev/mychar
