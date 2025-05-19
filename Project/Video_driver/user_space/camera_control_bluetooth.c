#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define DEVICE_FILE "/dev/video_driver"
#define VIDEO_FILE "output.mp4"
#define FFMPEG_PID_FILE "/tmp/ffmpeg.pid"

void send_command(const char *command) {
    int fd = open(DEVICE_FILE, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", DEVICE_FILE, strerror(errno));
        return;
    }
    if (write(fd, command, strlen(command)) < 0) {
        fprintf(stderr, "Failed to write to %s: %s\n", DEVICE_FILE, strerror(errno));
    } else {
        printf("Sent %s to kernel module\n", command);
    }
    close(fd);
}

void start_recording() {
    if (access(FFMPEG_PID_FILE, F_OK) == 0) {
        printf("Recording already in progress\n");
        return;
    }
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ffmpeg -i /dev/video0 -c:v libx264 -preset fast %s & echo $! > %s", VIDEO_FILE, FFMPEG_PID_FILE);
    if (system(cmd) == 0) {
        printf("Started recording\n");
    } else {
        fprintf(stderr, "Failed to start recording\n");
    }
}

void stop_recording() {
    if (access(FFMPEG_PID_FILE, F_OK) != 0) {
        printf("No recording in progress\n");
        return;
    }
    FILE *fp = fopen(FFMPEG_PID_FILE, "r");
    if (!fp) {
        fprintf(stderr, "Failed to read %s: %s\n", FFMPEG_PID_FILE, strerror(errno));
        return;
    }
    char pid_str[16];
    if (fgets(pid_str, sizeof(pid_str), fp)) {
        int pid = atoi(pid_str);
        if (pid > 0) {
            if (kill(pid, SIGTERM) == 0) {
                printf("Stopped recording\n");
                unlink(FFMPEG_PID_FILE);
            } else {
                fprintf(stderr, "Failed to kill ffmpeg: %s\n", strerror(errno));
            }
        }
    }
    fclose(fp);
}

int main() {
    char input[10];
    printf("Enter 1 to start, 0 to stop, or q to quit:\n");

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            continue;
        }
        input[strcspn(input, "\n")] = 0; // Remove newline

        if (strcmp(input, "1") == 0) {
            send_command("start");
            start_recording();
        } else if (strcmp(input, "0") == 0) {
            send_command("stop");
            stop_recording();
        } else if (strcmp(input, "q") == 0) {
            printf("Exiting\n");
            stop_recording(); // Ensure recording stops
            break;
        } else {
            printf("Invalid input. Use 1 (start), 0 (stop), or q (quit).\n");
        }
    }

    return 0;
}
