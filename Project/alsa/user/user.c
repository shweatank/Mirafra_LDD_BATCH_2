#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int play_wav(const char *filename) {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = 44100;
    int dir;
    snd_pcm_uframes_t frames = 32;
    int rc;

    // Open PCM device
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Cannot open PCM device: %s\n", snd_strerror(rc));
        return -1;
    }

    // Allocate and set hardware parameters
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "Cannot set HW params: %s\n", snd_strerror(rc));
        snd_pcm_close(handle);
        return -1;
    }

    // Open WAV file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Cannot open WAV file");
        snd_pcm_close(handle);
        return -1;
    }

    // Skip 44-byte WAV header
    fseek(fp, 44, SEEK_SET);

    // Playback loop
    char buffer[4096];
    int size;
    while ((size = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        rc = snd_pcm_writei(handle, buffer, size / 4); // 2 channels * 16 bits
        if (rc == -EPIPE) {
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "Write error: %s\n", snd_strerror(rc));
        }
    }

    fclose(fp);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <wav_file>\n", argv[0]);
        return 1;
    }

    int fd = open("/dev/audio_trigger", O_RDONLY);
    if (fd < 0) {
        perror("Cannot open /dev/audio_trigger");
        return 1;
    }

    char trigger;
    printf("Waiting for kernel trigger...\n");
    while (1) {
        if (read(fd, &trigger, 1) > 0) {
            if (trigger == '1') {
                printf("Trigger received, playing %s\n", argv[1]);
                play_wav(argv[1]);
            }
        }
    }

    close(fd);
    return 0;
}
