#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void write_to_file(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return;
    }
    write(fd, value, strlen(value));
    close(fd);
}

int main() {
    // Export GPIO17
    write_to_file("/sys/class/gpio/export", "17");
    sleep(1);

    // Set as output
    write_to_file("/sys/class/gpio/gpio17/direction", "out");

    // Blink LED
    for (int i = 0; i < 10; i++) {
        write_to_file("/sys/class/gpio/gpio17/value", "1");
        printf("LED ON\n");
        sleep(1);

        write_to_file("/sys/class/gpio/gpio17/value", "0");
        printf("LED OFF\n");
        sleep(1);
    }

    // Unexport
    write_to_file("/sys/class/gpio/unexport", "17");

    return 0;
}
