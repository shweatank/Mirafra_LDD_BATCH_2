#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define GPIO_NUMBER "17"  // Use GPIO17 (Pin 11)

void gpio_export(const char *gpio) {
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        perror("Export failed");
        return;
    }
    write(fd, gpio, strlen(gpio));
    close(fd);
}

void gpio_direction(const char *gpio, const char *direction) {
    char path[64];
    sprintf(path, "/sys/class/gpio/gpio%s/direction", gpio);
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Direction failed");
        return;
    }
    write(fd, direction, strlen(direction));
    close(fd);
}

void gpio_write(const char *gpio, const char *value) {
    char path[64];
    sprintf(path, "/sys/class/gpio/gpio%s/value", gpio);
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Write failed");
        return;
    }
    write(fd, value, strlen(value));
    close(fd);
}

void gpio_unexport(const char *gpio) {
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd < 0) {
        perror("Unexport failed");
        return;
    }
    write(fd, gpio, strlen(gpio));
    close(fd);
}

int main() {
    gpio_export(GPIO_NUMBER);
    sleep(1);  // Give time for sysfs to create files

    gpio_direction(GPIO_NUMBER, "out");

    printf("Turning LED ON\n");
    gpio_write(GPIO_NUMBER, "1");
    sleep(2);

    printf("Turning LED OFF\n");
    gpio_write(GPIO_NUMBER, "0");
    sleep(2);

    gpio_unexport(GPIO_NUMBER);
    return 0;
}

