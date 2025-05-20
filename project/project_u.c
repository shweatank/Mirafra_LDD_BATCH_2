#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void play_audio() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = 44100;
    int dir;
    snd_pcm_uframes_t frames = 32;
    int rc;

    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
        exit(1);
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    FILE *fp = fopen("test.wav", "rb");
    if (!fp) {
        perror("Failed to open WAV file");
        return;
    }

    fseek(fp, 44, SEEK_SET);  // skip header

    char buffer[4096];
    int size;
    while ((size = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        rc = snd_pcm_writei(handle, buffer, size / 4);
        if (rc == -EPIPE) {
            fprintf(stderr, "underrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
        }
    }

    fclose(fp);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
}

int main() {
    int fd = open("/dev/project_k", O_WRONLY);
    if (fd < 0) {
        perror("Open /dev/audio_trigger");
        return 1;
    }

    printf("Press 'p' to play audio. Press 'q' to quit.\n");

    while (1) {
        int ch = getch();
        if (ch == 'p') {
            write(fd, "1", 1);  // send trigger to kernel
            play_audio();       // then play audio
        } else if (ch == 'q') {
            break;
        }
    }

    close(fd);
    return 0;
}

