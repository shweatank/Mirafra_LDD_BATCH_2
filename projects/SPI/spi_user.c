// spi_user_space.c
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <string.h>

#define SPI_PATH "/dev/spidev0.0"

int main(void)
{
    int fd;
    int ret;
    uint8_t tx[] = {0x01, 0x02, 0x03};  // Data to send
    uint8_t rx[3];  // Buffer to receive data

    // Open SPI device
    fd = open(SPI_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open SPI device");
        return 1;
    }

    // Set SPI mode
    uint8_t mode = SPI_MODE_0;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        perror("Failed to set SPI mode");
        close(fd);
        return 1;
    }

    // Set SPI bits per word
    uint8_t bits_per_word = 8;
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
    if (ret == -1) {
        perror("Failed to set bits per word");
        close(fd);
        return 1;
    }

    // Set SPI speed (in Hz)
    uint32_t speed = 500000;
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        perror("Failed to set max speed");
        close(fd);
        return 1;
    }

    // Perform SPI transaction
    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = sizeof(tx),
        .speed_hz = speed,
        .bits_per_word = bits_per_word,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
    if (ret < 1) {
        perror("Failed to send SPI message");
        close(fd);
        return 1;
    }

    // Print received data
    printf("Received data: ");
    for (int i = 0; i < sizeof(rx); i++) {
        printf("0x%02x ", rx[i]);
    }
    printf("\n");

    close(fd);
    return 0;
}

