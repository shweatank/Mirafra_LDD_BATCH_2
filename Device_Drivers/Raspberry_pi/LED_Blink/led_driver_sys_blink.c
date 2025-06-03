#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define LED_PATH "/sys/class/leds/ACT"
#define TRIGGER_FILE LED_PATH "/trigger"
#define BRIGHTNESS_FILE LED_PATH "/brightness"

void set_trigger(const char *trigger) {
    int fd = open(TRIGGER_FILE, O_WRONLY);
    if (fd < 0) {
        perror("Error opening trigger file");
        exit(1);
    }
    if (write(fd, trigger, strlen(trigger)) < 0) {
        perror("Error writing to trigger file");
        close(fd);
        exit(1);
    }
    close(fd);
}

void set_brightness(int value) {
    char buf[2];
    snprintf(buf, sizeof(buf), "%d", value);
    int fd = open(BRIGHTNESS_FILE, O_WRONLY);
    if (fd < 0) {
        perror("Error opening brightness file");
        exit(1);
    }
    if (write(fd, buf, strlen(buf)) < 0) {
        perror("Error writing to brightness file");
        close(fd);
        exit(1);
    }
    close(fd);
}

void cleanup(int signum) {
    set_trigger("mmc0"); // Restore default trigger
    printf("\nRestored LED trigger. Exiting.\n");
    exit(0);
}

int main() {
    // Set up signal handler for Ctrl+C
    signal(SIGINT, cleanup);

    // Disable default trigger
    set_trigger("none");
    printf("Blinking ACT LED. Press Ctrl+C to stop.\n");

    while (1) {
        set_brightness(1); // Turn LED on
        sleep(1);
        set_brightness(0); // Turn LED off
        sleep(1);
    }

    return 0; // Unreachable
}
