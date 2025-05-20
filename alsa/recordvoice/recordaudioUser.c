#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define PCM_DEVICE "default"
#define SAMPLE_RATE 44100
#define CHANNELS 1
#define FORMAT SND_PCM_FORMAT_S16_LE
#define RECORD_SECONDS 5
#define FRAMES_PER_BUFFER 32

int main() {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    snd_pcm_uframes_t frames = FRAMES_PER_BUFFER;
    char *buffer;
    int rc;
    int size;

    FILE *output = fopen("recording.raw", "wb");
    if (!output) {
        perror("fopen");
        return 1;
    }

    // Open PCM device for recording (capture)
    rc = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open PCM device: %s\n", snd_strerror(rc));
        return 1;
    }

    // Allocate HW params
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(pcm_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle, params, SAMPLE_RATE, 0);
    snd_pcm_hw_params_set_period_size(pcm_handle, params, frames, 0);

    // Apply HW parameter settings
    rc = snd_pcm_hw_params(pcm_handle, params);
    if (rc < 0) {
        fprintf(stderr, "Unable to set HW parameters: %s\n", snd_strerror(rc));
        return 1;
    }

    snd_pcm_hw_params_get_period_size(params, &frames, 0);
    size = frames * CHANNELS * 2; // 2 bytes per sample (S16_LE)
    buffer = (char *) malloc(size);

    int loops = (SAMPLE_RATE * RECORD_SECONDS) / frames;

    while (loops-- > 0) {
        rc = snd_pcm_readi(pcm_handle, buffer, frames);
        if (rc == -EPIPE) {
            fprintf(stderr, "Overrun occurred\n");
            snd_pcm_prepare(pcm_handle);
        } else if (rc < 0) {
            fprintf(stderr, "Error from read: %s\n", snd_strerror(rc));
        } else if (rc != (int)frames) {
            fprintf(stderr, "Short read: read %d frames\n", rc);
        }
        fwrite(buffer, size, 1, output);
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buffer);
    fclose(output);

    printf("Recording saved to 'recording.raw'\n");

    return 0;
}

