#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define PCM_DEVICE "default"

int main() {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = 44100;
    int channels = 2;
    snd_pcm_uframes_t frames = 32;
    int rc;
    char *buffer;
    int size;

    FILE *fp = fopen("sound.raw", "rb");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    rc = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, NULL);
    snd_pcm_hw_params(pcm_handle, params);
    snd_pcm_hw_params_get_period_size(params, &frames, NULL);

    size = frames * channels * 2; // 2 bytes/sample for S16_LE
    buffer = (char *) malloc(size);

    while ((rc = fread(buffer, 1, size, fp)) > 0) {
        rc = snd_pcm_writei(pcm_handle, buffer, frames);
        if (rc == -EPIPE) {
            snd_pcm_prepare(pcm_handle);
        } else if (rc < 0) {
            fprintf(stderr, "error writing to PCM device: %s\n", snd_strerror(rc));
        }
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buffer);
    fclose(fp);

    return 0;
}

