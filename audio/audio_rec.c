
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#define PCM_DEVICE "default"
#define CHANNELS 2
#define RATE 44100
#define FORMAT SND_PCM_FORMAT_S16_LE
#define RECORD_SECONDS 5
#define FRAMES 32

void write_wav_header(FILE *file, int sample_rate, short channels, int data_length) {
    int byte_rate = sample_rate * channels * 2;
    short block_align = channels * 2;
    short bits_per_sample = 16;
    
    fwrite("RIFF", 1, 4, file);
    int chunk_size = 36 + data_length;
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);
    fwrite("fmt ", 1, 4, file);
    int subchunk1_size = 16;
    fwrite(&subchunk1_size, 4, 1, file);
    short audio_format = 1;
    fwrite(&audio_format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits_per_sample, 2, 1, file);
    fwrite("data", 1, 4, file);
    fwrite(&data_length, 4, 1, file);
}

int main() {
    snd_pcm_t *capture_handle, *playback_handle;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_uframes_t frames = FRAMES;
    unsigned int sample_rate = RATE;
    int dir, rc;

    int buffer_size = FRAMES * CHANNELS * 2; // 2 bytes per sample
    char *buffer = (char *)malloc(buffer_size);
    FILE *fp = fopen("recorded.wav", "wb");

    if (!fp) {
        fprintf(stderr, "Failed to open file for recording\n");
        return 1;
    }

    // === CAPTURE SETUP ===
    rc = snd_pcm_open(&capture_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Error opening capture device: %s\n", snd_strerror(rc));
        return 1;
    }

    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, FORMAT);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &frames, &dir);
    snd_pcm_hw_params(capture_handle, hw_params);

    // Write temporary WAV header
    int data_length = RATE * CHANNELS * 2 * RECORD_SECONDS;
    write_wav_header(fp, RATE, CHANNELS, data_length);

    printf("Recording for %d seconds...\n", RECORD_SECONDS);

    int loops = RATE / FRAMES * RECORD_SECONDS;

    while (loops--) {
        rc = snd_pcm_readi(capture_handle, buffer, frames);
        if (rc == -EPIPE) {
            snd_pcm_prepare(capture_handle);
            continue;
        } else if (rc < 0) {
            fprintf(stderr, "Read error: %s\n", snd_strerror(rc));
            break;
        }

        fwrite(buffer, 1, buffer_size, fp);
    }

    fclose(fp);
    snd_pcm_close(capture_handle);
    printf("Recording complete and saved to 'recorded.wav'.\n");

    // === PLAYBACK SETUP ===
    rc = snd_pcm_open(&playback_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Error opening playback device: %s\n", snd_strerror(rc));
        return 1;
    }

    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_hw_params_any(playback_handle, hw_params);
    snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, hw_params, FORMAT);
    snd_pcm_hw_params_set_channels(playback_handle, hw_params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &frames, &dir);
    snd_pcm_hw_params(playback_handle, hw_params);

    // === PLAYBACK ===
    fp = fopen("recorded.wav", "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open recorded file\n");
        return 1;
    }

    fseek(fp, 44, SEEK_SET); // Skip WAV header
    while ((rc = fread(buffer, 1, buffer_size, fp)) > 0) {
        snd_pcm_writei(playback_handle, buffer, frames);
    }

    fclose(fp);
    snd_pcm_drain(playback_handle);
    snd_pcm_close(playback_handle);
    free(buffer);

    printf("Playback complete.\n");
    return 0;
}
