#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define INPUT_PATH "/tmp/key_audio_trigger"
#define BASE_PATH "./Keys_Piono"

char *audio_map[26] = {
    "a0.wav", "a1.wav", "a2.wav", "a3.wav", "a4.wav", "a5.wav", "a6.wav", "a7.wav",   // a-h
    "b0.wav", "b1.wav", "b2.wav", "b3.wav", "b4.wav", "b5.wav", "b6.wav", "b7.wav",   // i-p
    "c1.wav", "c2.wav", "c3.wav", "c4.wav", "c5.wav", "c6.wav", "c7.wav", "c8.wav",   // q-x
    "d1.wav", "d2.wav"                                                               // y-z
};

void play_sound(char key) {
    if (key < 'a' || key > 'z') return;

    char command[512];
    snprintf(command, sizeof(command), "aplay %s/%s > /dev/null 2>&1", BASE_PATH, audio_map[key - 'a']);
    system(command);
}

int main() {
    printf("🔊 Key Audio Daemon Started. Listening to %s...\n", INPUT_PATH);

    while (1) {
        int fd = open(INPUT_PATH, O_RDONLY);
        if (fd < 0) {
            perror("open");
            sleep(1);
            continue;
        }

        char key = 0;
        read(fd, &key, 1);
        close(fd);

        if (key >= 'A' && key <= 'Z') key += 32; // convert to lowercase
        printf("Key '%c' pressed. Playing audio...\n", key);
        play_sound(key);
        unlink(INPUT_PATH);  // clean up after reading
	//close(fd);
    }

    return 0;
}

