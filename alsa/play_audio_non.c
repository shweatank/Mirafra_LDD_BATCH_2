#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define FRAMES 64
#define SAMPLE_RATE 44100

int main() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    FILE *fp;
    int err;
    size_t size;
    int rc;
    int16_t *left_buffer, *right_buffer;
    void *buffers[2];

    // Open PCM device
    err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        fprintf(stderr, "Error opening PCM device: %s\n", snd_strerror(err));
        exit(1);
    }

    // Allocate hardware parameters object and configure
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_NONINTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);  // stereo
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, 0);
    snd_pcm_uframes_t frames = FRAMES;
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, 0);
    err = snd_pcm_hw_params(handle, params);
    if (err < 0) {
        fprintf(stderr, "Error setting hardware parameters: %s\n", snd_strerror(err));
        exit(1);
    }

    // Open audio file
    fp = fopen("test1.wav", "rb");
    if (!fp) {
        perror("Error opening file");
        exit(1);
    }

    // Allocate buffers
    left_buffer = malloc(frames * sizeof(int16_t));
    right_buffer = malloc(frames * sizeof(int16_t));
    if (!left_buffer || !right_buffer) {
        fprintf(stderr, "Error allocating memory for buffers\n");
        exit(1);
    }
    buffers[0] = left_buffer;
    buffers[1] = right_buffer;

    // Playback loop
    size_t frame_size = 2 * sizeof(int16_t);  // 2 channels * 16-bit samples
    while (1) {
        // Read left and right channels
        size = fread(left_buffer, frame_size, frames, fp);
        if (size == 0) {
            break;  // End of file, exit the loop
        }

        // Read right channel data for stereo playback
        size_t right_size = fread(right_buffer, frame_size, frames, fp);
        if (right_size != size) {
            // If we haven't read the same amount of data for both channels, handle it.
            fprintf(stderr, "Error reading right channel data\n");
            break;
        }

        // Write data to PCM device
        rc = snd_pcm_writen(handle, buffers, size);
        if (rc == -EPIPE) {
            // Buffer underrun
            fprintf(stderr, "Underrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "Error from write: %s\n", snd_strerror(rc));
            break;
        }
    }


    // Clean up and close
    fclose(fp);
    free(left_buffer);
    free(right_buffer);
    snd_pcm_close(handle);

    return 0;
}

