#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define I2C_DEV "/dev/i2c-1"
#define MAX30102_ADDR 0x57

// MAX30102 registers
#define REG_PART_ID         0xFF
#define REG_REV_ID          0xFE
#define REG_FIFO_DATA       0x07
#define REG_FIFO_WR_PTR     0x04
#define REG_FIFO_RD_PTR     0x06
#define REG_FIFO_OVF_CNT    0x05
#define REG_MODE_CONFIG     0x09
#define REG_SPO2_CONFIG     0x0A
#define REG_LED1_PA         0x0C  // Red LED pulse amplitude
#define REG_LED2_PA         0x0D  // IR LED pulse amplitude
#define REG_TEMP_INT        0x1F
#define REG_TEMP_FRAC       0x20
#define REG_RESET           0x0F

// Basic write to a register
int i2c_write_reg(int fd, unsigned char reg, unsigned char val) {
    unsigned char buf[2] = {reg, val};
    if (write(fd, buf, 2) != 2) {
        perror("Failed to write reg");
        return -1;
    }
    return 0;
}

// Basic read from a register
int i2c_read_reg(int fd, unsigned char reg) {
    if (write(fd, &reg, 1) != 1) {
        perror("Failed to write reg address");
        return -1;
    }
    unsigned char val;
    if (read(fd, &val, 1) != 1) {
        perror("Failed to read reg");
        return -1;
    }
    return val;
}

// Read multiple bytes from FIFO
int i2c_read_fifo(int fd, unsigned char *buf, int len) {
    unsigned char reg = REG_FIFO_DATA;
    if (write(fd, &reg, 1) != 1) {
        perror("Failed to write FIFO reg");
        return -1;
    }
    if (read(fd, buf, len) != len) {
        perror("Failed to read FIFO data");
        return -1;
    }
    return 0;
}

// Initialize sensor with basic settings
int max30102_init(int fd) {
    // Reset device
    if (i2c_write_reg(fd, REG_MODE_CONFIG, 0x40) < 0) return -1;
    usleep(100000);

    // FIFO config: sample averaging = 4, FIFO rollover enabled
    if (i2c_write_reg(fd, 0x08, 0x4F) < 0) return -1;

    // SpO2 config: ADC range = 4096nA, sample rate = 100 Hz, pulse width = 411 us
    if (i2c_write_reg(fd, REG_SPO2_CONFIG, 0x27) < 0) return -1;

    // LED Pulse amplitude: Red = 0x24 (36), IR = 0x24 (36)
    if (i2c_write_reg(fd, REG_LED1_PA, 0x24) < 0) return -1;
    if (i2c_write_reg(fd, REG_LED2_PA, 0x24) < 0) return -1;

    // Mode config: SpO2 mode
    if (i2c_write_reg(fd, REG_MODE_CONFIG, 0x03) < 0) return -1;

    return 0;
}

int main() {
    int fd = open(I2C_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, MAX30102_ADDR) < 0) {
        perror("Failed to set I2C address");
        close(fd);
        return 1;
    }

    int part_id = i2c_read_reg(fd, REG_PART_ID);
    if (part_id != 0x15) {
        printf("Unexpected PART ID: 0x%02X\n", part_id);
        close(fd);
        return 1;
    }
    printf("MAX30102 detected with PART ID: 0x%02X\n", part_id);

    if (max30102_init(fd) < 0) {
        printf("Failed to initialize MAX30102\n");
        close(fd);
        return 1;
    }
    printf("MAX30102 initialized successfully\n");

    while (1) {
        unsigned char buf[6];
        // Each sample is 3 bytes, red and IR = 6 bytes
        if (i2c_read_fifo(fd, buf, 6) < 0) {
            printf("Failed to read FIFO\n");
            break;
        }

        // Combine bytes (18-bit values, highest 6 bits are zero)
        int red = ((buf[0] << 16) | (buf[1] << 8) | buf[2]) & 0x3FFFF;
        int ir = ((buf[3] << 16) | (buf[4] << 8) | buf[5]) & 0x3FFFF;

        printf("Red LED: %d, IR LED: %d\n", red, ir);

        usleep(500000);  // 500ms delay
    }

    close(fd);
    return 0;
}
