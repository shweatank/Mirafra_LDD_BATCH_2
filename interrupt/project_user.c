#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE "/dev/project"
#define MAGIC_NUM 100
#define IOCTL_GET_PANIC_FLAG _IOR(MAGIC_NUM, 1, int *)

int main() {
    int fd, panic_flag = 0;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open panic_button device");
        return EXIT_FAILURE;
    }

    printf("Press 'Esc' key 3 times to trigger panic (within short interval)...\n");

    for (int i = 0; i < 10; i++) {
        sleep(1); // Wait for user input
        ioctl(fd, IOCTL_GET_PANIC_FLAG, &panic_flag);
        if (panic_flag) {
            printf("🚨 Panic Detected! (Triple Esc)\n");
            break;
        }
    }

    if (!panic_flag)
        printf("No panic detected in time window.\n");

    close(fd);
    return 0;
}

