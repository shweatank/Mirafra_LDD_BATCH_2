#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SERIAL_PORT "/dev/rpi_uart"
#define LCD_DEVICE "/dev/lcd"

int main() {
    int fd_serial, fd_lcd;
    char buf[256];
    char user_msg[256];
    ssize_t n;

   // const char *response = "";

    /* Open serial port */
    fd_serial = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd_serial < 0) {
        perror("Failed to open serial port");
        exit(1);
    }

    /* Set non-blocking read */
    if (fcntl(fd_serial, F_SETFL, O_NONBLOCK) < 0) {
        perror("Failed to set non-blocking");
        close(fd_serial);
        exit(1);
    }

    /* Open LCD device */
    fd_lcd = open(LCD_DEVICE, O_WRONLY);
    if (fd_lcd < 0) {
        perror("Failed to open LCD device");
        close(fd_serial);
        exit(1);
    }

    printf("Serial and LCD devices opened.\n");

    while (1) {
        /* Check for incoming serial data */
        n = read(fd_serial, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Received from UART: %s", buf);

            /* Write to LCD */
            ssize_t lcd_written = write(fd_lcd, buf, n);
            if (lcd_written < 0) {
                perror("Failed to write to LCD");
            } else {
                printf("Written to LCD: %s", buf);
            }


        /* Prompt user to send message */
        printf("\nEnter message to send over UART (or press Enter to skip): ");
        fflush(stdout);

        if (fgets(user_msg, sizeof(user_msg), stdin) != NULL) {
            if (strlen(user_msg) > 1) { // check if not just newline
                n = write(fd_serial, user_msg, strlen(user_msg));
                if (n < 0) {
                    perror("Write error");
                    break;
                }
                printf("Sent: %s", user_msg);
            }
        }

        sleep(1);  // Small delay to avoid CPU hogging
    }

    close(fd_serial);
    close(fd_lcd);
    return 0;
}

