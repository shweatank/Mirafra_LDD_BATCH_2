#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define BITS_PER_SAMPLE 16
#define DURATION_SECONDS 15

typedef struct {
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
    char subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char subchunk2_id[4];
    uint32_t subchunk2_size;
} WavHeader;

void write_wav_header(FILE *fp, uint32_t data_size) {
    WavHeader header = {
        .chunk_id = "RIFF",
        .format = "WAVE",
        .subchunk1_id = "fmt ",
        .subchunk1_size = 16,
        .audio_format = 1,
        .num_channels = CHANNELS,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = BITS_PER_SAMPLE,
        .subchunk2_id = "data"
    };
    header.byte_rate = SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8);
    header.block_align = CHANNELS * (BITS_PER_SAMPLE / 8);
    header.subchunk2_size = data_size;
    header.chunk_size = 36 + data_size;
    fwrite(&header, sizeof(WavHeader), 1, fp);
}

int main() {
    snd_pcm_t *capture_handle = NULL, *playback_handle = NULL;
    snd_pcm_hw_params_t *params = NULL;
    int rc, dir = 0;
    snd_pcm_uframes_t frames = 64;
    int16_t buffer[8192];
    FILE *fp = NULL;
    uint32_t samples_to_record = SAMPLE_RATE * DURATION_SECONDS;
    uint32_t data_size = samples_to_record * CHANNELS * (BITS_PER_SAMPLE / 8);

    // Open capture device
    rc = snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open capture device: %s\n", snd_strerror(rc));
        return 1;
    }

    // Open playback device
    rc = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open playback device: %s\n", snd_strerror(rc));
        goto cleanup;
    }

    // Set capture parameters
    snd_pcm_hw_params_alloca(¶ms);
    snd_pcm_hw_params_any(capture_handle, params);
    snd_pcm_hw_params_set_access(capture_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(capture_handle, params, CHANNELS);
    unsigned int rate = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(capture_handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size_near(capture_handle, params, &frames, &dir);
    rc = snd_pcm_hw_params(capture_handle, params);
    if (rc < 0) {
        fprintf(stderr, "Unable to set capture parameters: %s\n", snd_strerror(rc));
        goto cleanup;
    }

    // Set playback parameters
    snd_pcm_hw_params_alloca(¶ms);
    snd_pcm_hw_params_any(playback_handle, params);
    snd_pcm_hw_params_set_access(playback_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(playback_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(playback_handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size_near(playback_handle, params, &frames, &dir);
    rc = snd_pcm_hw_params(playback_handle, params);
    if (rc < 0) {
        fprintf(stderr, "Unable to set playback parameters: %s\n", snd_strerror(rc));
        goto cleanup;
    }

    // Open output WAV file
    fp = fopen("output.wav", "wb");
    if (!fp) {
        perror("Failed to open output WAV file");
        goto cleanup;
    }
    write_wav_header(fp, data_size);

    // Record and play simultaneously
    printf("Recording and playing for %d seconds...\n", DURATION_SECONDS);
    uint32_t samples_processed = 0;
    while (samples_processed < samples_to_record) {
        size_t frames_to_read = (8192 / CHANNELS < (samples_to_record - samples_processed)) ? 8192 / CHANNELS : (samples_to_record - samples_processed);
        rc = snd_pcm_readi(capture_handle, buffer, frames_to_read);
        if (rc < 0) {
            fprintf(stderr, "Read error: %s\n", snd_strerror(rc));
            if (rc == -EPIPE) snd_pcm_prepare(capture_handle);
            continue;
        }

        // Write to file
        fwrite(buffer, sizeof(int16_t), rc * CHANNELS, fp);

        // Play immediately
        rc = snd_pcm_writei(playback_handle, buffer, rc);
        if (rc == -EPIPE) {
            snd_pcm_prepare(playback_handle);
        } else if (rc < 0) {
            fprintf(stderr, "Write error: %s\n", snd_strerror(rc));
            break;
        }

        samples_processed += rc;
    }

cleanup:
    if (fp) fclose(fp);
    if (capture_handle) snd_pcm_close(capture_handle);
    if (playback_handle) {
        snd_pcm_drain(playback_handle);
        snd_pcm_close(playback_handle);
    }
    return rc < 0 ? 1 : 0;
}
