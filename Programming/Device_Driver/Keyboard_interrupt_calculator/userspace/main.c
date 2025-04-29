// user_write_nums.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    int fd, a, b;
    char buf[64];

    printf("Enter two integers: ");
    if (scanf("%d %d", &a, &b) != 2) {
        fprintf(stderr, "Bad input\n");
        return 1;
    }

    fd = open("/dev/keyboard_device", O_WRONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    snprintf(buf, sizeof(buf), "%d %d", a, b);
    if (write(fd, buf, strlen(buf)) < 0) {
        perror("write");
        close(fd);
        return 1;
    }

    close(fd);
    printf("Sent %d and %d to kernel; now press keys and watch dmesg.\n",
           a, b);
    return 0;
}
