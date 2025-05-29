#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    FILE *f;
    char command[128];

    while (1) {
        f = fopen("/dev/my_kernel_dev", "r");  // Your char device path
        if (!f) {
            perror("Open failed");
            sleep(1);
            continue;
        }

        fgets(command, sizeof(command), f);
        fclose(f);

        if (strcmp(command, "3") == 0) {
            system("ffplay -autoexit -fs my_video.mp4");
        } else if (strcmp(command, "4") == 0) {
            system("pkill ffplay"); // Stop playback
        }

        sleep(1); // Avoid tight loop
    }

    return 0;
}

