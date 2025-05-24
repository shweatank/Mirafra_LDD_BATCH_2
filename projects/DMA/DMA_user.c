:// File: trigger_dma_gpio.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/test", O_WRONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *msg = "1";
    if (write(fd, msg, 1) < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    printf("DMA GPIO LED triggered\n");
    close(fd);
    return 0;
}
