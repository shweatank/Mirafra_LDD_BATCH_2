#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buffer[4096];
    int fd = open("/dev/key_count", O_RDONLY);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    read(fd, buffer, sizeof(buffer));
    printf("\nKey Counts from Kernel:\n%s\n", buffer);

    close(fd);
    return 0;
}

/*

OUTPUT:

Key Counts from Kernel:
Key Press Count:
Backspace pressed 14 times
Tab pressed 2 times
Q pressed 1 times
W pressed 1 times
E pressed 1 times
R pressed 2 times
T pressed 1 times
Enter pressed 4 times
. pressed 2 times
/ pressed 2 times
Up Arrow pressed 30 times


   */
