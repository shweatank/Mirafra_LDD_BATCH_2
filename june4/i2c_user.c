#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int i2c_open(const char *bus, int addr) {
    int file;

    if ((file = open(bus, O_RDWR)) < 0) {
        perror("Failed to open I2C bus");
        return -1;
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Failed to set I2C slave address");
        close(file);
        return -1;
    }

    return file;
}

int i2c_write_byte(int file, uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    if (write(file, buffer, 2) != 2) {
        perror("Failed to write to I2C device");
        return -1;
    }
    return 0;
}

int i2c_read_byte(int file, uint8_t reg, uint8_t *data) {
    if (write(file, &reg, 1) != 1) {
        perror("Failed to write register address");
        return -1;
    }

    if (read(file, data, 1) != 1) {
        perror("Failed to read from I2C device");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    const char *bus = "/dev/i2c-1"; // or /dev/i2c-1 etc
    int addr = 0x08; // Example: EEPROM I2C address
    int file = i2c_open(bus, addr);

    if (file < 0) return 1;

    uint8_t data;
    if (i2c_read_byte(file, 0x00, &data) == 0) {
        printf("Read byte: 0x%02x\n", data);
    }

    i2c_write_byte(file, 0x00, 0xAB);

    close(file);
    return 0;
}
