#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/mmap_dev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    char *map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    printf("Data from kernel: %s\n", map);
    munmap(map, 4096);
    close(fd);
    return 0;
}

