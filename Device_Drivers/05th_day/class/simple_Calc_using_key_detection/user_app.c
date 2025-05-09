#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define DEVICE_PATH "/dev/keyboard_interrupt" // Path to the device file created by the kernel module

// Function to configure terminal to capture input without pressing enter
void set_terminal_mode() {
    struct termios new_settings;
    tcgetattr(STDIN_FILENO, &new_settings);
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

// Function to reset terminal to its default mode
void reset_terminal_mode() {
    struct termios new_settings;
    tcgetattr(STDIN_FILENO, &new_settings);
    new_settings.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

int main() {
    char c;
    int fd;

    // Open the device file for the kernel module
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Failed to open the device file");
        return 1;
    }

    // Set terminal mode to capture keypresses without pressing enter
    set_terminal_mode();

    printf("Press 'a' to perform operations. Press 'q' to quit.\n");

    while (1) {
        c = getchar();  // Read a single character input from the user

        if (c == 'q') {
            break; // Exit the loop and the program when 'q' is pressed
        }

        if (c == 'a') {
            // Send a signal to the kernel module every time 'a' is pressed
            write(fd, &c, sizeof(c));
        }
    }

    // Reset the terminal to the default mode
    reset_terminal_mode();

    // Close the device file
    close(fd);
    return 0;
}

