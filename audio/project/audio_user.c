#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <alsa/asoundlib.h>

#define DEVICE "/dev/kbd_notify"
#define PCM_DEVICE "default"
#define RECORD_SECONDS 5
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define BUFFER_SIZE (SAMPLE_RATE * RECORD_SECONDS)

short buffer[BUFFER_SIZE];

void record_audio() {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle, params, SAMPLE_RATE, 0);
    snd_pcm_hw_params(pcm_handle, params);
    snd_pcm_prepare(pcm_handle);

    printf("🎙️  Recording for %d seconds...\n", RECORD_SECONDS);
    snd_pcm_readi(pcm_handle, buffer, BUFFER_SIZE);
    snd_pcm_close(pcm_handle);
    printf("✅ Done recording\n");
}

void play_audio() {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle, params, SAMPLE_RATE, 0);
    snd_pcm_hw_params(pcm_handle, params);
    snd_pcm_prepare(pcm_handle);

    printf("🔊 Playing back...\n");
    snd_pcm_writei(pcm_handle, buffer, BUFFER_SIZE);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    printf("✅ Done playing\n");
}

int main() {
    int fd = open(DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("open device");
        return 1;
    }

    char buf;
    printf("Waiting for 'V' key press from kernel module...\n");

    while (1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        int ret = select(fd + 1, &rfds, NULL, NULL, NULL);
        if (ret > 0 && FD_ISSET(fd, &rfds)) {
            int r = read(fd, &buf, 1);
            if (r == 1 && buf == 'V') {
                printf("Received 'V' key press event!\n");
                record_audio();
                play_audio();
            }
        }
    }

    close(fd);
    return 0;
}

