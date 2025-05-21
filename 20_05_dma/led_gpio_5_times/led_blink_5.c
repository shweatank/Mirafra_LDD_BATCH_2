#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define GPIO_BASE_PHYS  0xFE200000  // BCM2711 physical GPIO base
#define GPIO_LEN        0xB4        // Size of GPIO region to map

// Offsets from base
#define GPFSEL1         0x04
#define GPSET0          0x1C
#define GPCLR0          0x28

#define GPIO_PIN        18  // LED connected to GPIO18

int main() {
    int mem_fd;
    volatile uint32_t *gpio_base;

    // Open /dev/mem
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) {
        perror("open /dev/mem");
        return 1;
    }

    // Memory-map GPIO
    gpio_base = (volatile uint32_t *)mmap(NULL, GPIO_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE_PHYS);
    if (gpio_base == MAP_FAILED) {
        perror("mmap failed");
        close(mem_fd);
        return 1;
    }

    // Configure GPIO18 as output
    uint32_t val = gpio_base[GPFSEL1 / 4];
    val &= ~(0x7 << 24);       // Clear bits 26:24 for GPIO18
    val |= (0x1 << 24);        // Set output mode (001)
    gpio_base[GPFSEL1 / 4] = val;

    // Blink LED 5 times
    for (int i = 0; i < 5; i++) {
        gpio_base[GPSET0 / 4] = (1 << GPIO_PIN);   // LED ON
        usleep(500000);                            // 500 ms

        gpio_base[GPCLR0 / 4] = (1 << GPIO_PIN);   // LED OFF
        usleep(500000);                            // 500 ms
    }

    // Turn LED off before exit
    gpio_base[GPCLR0 / 4] = (1 << GPIO_PIN);

    // Cleanup
    munmap((void *)gpio_base, GPIO_LEN);
    close(mem_fd);

    return 0;
}
