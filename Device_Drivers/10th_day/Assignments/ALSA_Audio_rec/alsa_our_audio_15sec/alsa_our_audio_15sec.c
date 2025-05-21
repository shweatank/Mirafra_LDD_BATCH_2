#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define BITS_PER_SAMPLE 16
#define DURATION_SECONDS 15
#define BUFFER_FRAMES 4096 // Buffer holds 4096 frames (8192 samples for stereo)

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
    snd_pcm_hw_params_t *capture_params = NULL, *playback_params = NULL;
    int rc, dir = 0;
    snd_pcm_uframes_t frames = 64; // Small period size for low latency
    int16_t buffer[BUFFER_FRAMES * CHANNELS]; // Buffer for 4096 frames
    FILE *fp = NULL;
    uint32_t samples_to_record = SAMPLE_RATE * DURATION_SECONDS;
    uint32_t data_size = samples_to_record * CHANNELS * (BITS_PER_SAMPLE / 8);
    unsigned int rate = SAMPLE_RATE;

    // Open capture device
    rc = snd_pcm_open(&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open capture device: %s\n", snd_strerror(rc));
        return 1;
    }

    // Allocate and configure capture parameters
    snd_pcm_hw_params_alloca(&capture_params);
    rc = snd_pcm_hw_params_any(capture_handle, capture_params);
    if (rc < 0) {
        fprintf(stderr, "Unable to initialize capture parameters: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_access(capture_handle, capture_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc < 0) {
        fprintf(stderr, "Unable to set capture access: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_format(capture_handle, capture_params, SND_PCM_FORMAT_S16_LE);
    if (rc < 0) {
        fprintf(stderr, "Unable to set capture format: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_channels(capture_handle, capture_params, CHANNELS);
    if (rc < 0) {
        fprintf(stderr, "Unable to set capture channels: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_rate_near(capture_handle, capture_params, &rate, &dir);
    if (rc < 0) {
        fprintf(stderr, "Unable to set capture sample rate: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    if (rate != SAMPLE_RATE) {
        fprintf(stderr, "Warning: Capture sample rate set to %u Hz instead of %u Hz\n", rate, SAMPLE_RATE);
    }
    rc = snd_pcm_hw_params_set_period_size_near(capture_handle, capture_params, &frames, &dir);
    if (rc < 0) {
        fprintf(stderr, "Unable to set capture period size: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params(capture_handle, capture_params);
    if (rc < 0) {
        fprintf(stderr, "Unable to apply capture parameters: %s\n", snd_strerror(rc));
        goto cleanup;
    }

    // Open output WAV file
    fp = fopen("output.wav", "wb");
    if (!fp) {
        perror("Failed to open output WAV file");
        goto cleanup;
    }
    write_wav_header(fp, data_size);

    // Wait for user input to start recording
    printf("Press Enter to start recording for %d seconds...\n", DURATION_SECONDS);
    getchar();

    // Start capture
    rc = snd_pcm_start(capture_handle);
    if (rc < 0) {
        fprintf(stderr, "Unable to start capture: %s\n", snd_strerror(rc));
        goto cleanup;
    }

    // Record for 15 seconds
    printf("Recording...\n");
    uint32_t samples_processed = 0;
    while (samples_processed < samples_to_record) {
        snd_pcm_uframes_t frames_to_read = (BUFFER_FRAMES < (samples_to_record - samples_processed) / CHANNELS)
                                           ? BUFFER_FRAMES
                                           : (samples_to_record - samples_processed) / CHANNELS;

        rc = snd_pcm_readi(capture_handle, buffer, frames_to_read);
        if (rc < 0) {
            fprintf(stderr, "Read error: %s\n", snd_strerror(rc));
            if (rc == -EPIPE) {
                rc = snd_pcm_prepare(capture_handle);
                if (rc < 0) {
                    fprintf(stderr, "Failed to recover from capture overrun: %s\n", snd_strerror(rc));
                    goto cleanup;
                }
                continue;
            }
            goto cleanup;
        }

        // Write to file
        size_t written = fwrite(buffer, sizeof(int16_t), rc * CHANNELS, fp);
        if (written != rc * CHANNELS) {
            fprintf(stderr, "Failed to write to WAV file\n");
            goto cleanup;
        }

        samples_processed += rc * CHANNELS; // Increment by samples (frames * channels)
    }
    printf("Recording complete.\n");

    // Close the WAV file
    if (fp) {
        fclose(fp);
        fp = NULL;
    }

    // Open playback device
    rc = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open playback device: %s\n", snd_strerror(rc));
        goto cleanup;
    }

    // Allocate and configure playback parameters
    snd_pcm_hw_params_alloca(&playback_params);
    rc = snd_pcm_hw_params_any(playback_handle, playback_params);
    if (rc < 0) {
        fprintf(stderr, "Unable to initialize playback parameters: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_access(playback_handle, playback_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (rc < 0) {
        fprintf(stderr, "Unable to set playback access: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_format(playback_handle, playback_params, SND_PCM_FORMAT_S16_LE);
    if (rc < 0) {
        fprintf(stderr, "Unable to set playback format: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_channels(playback_handle, playback_params, CHANNELS);
    if (rc < 0) {
        fprintf(stderr, "Unable to set playback channels: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params_set_rate_near(playback_handle, playback_params, &rate, &dir);
    if (rc < 0) {
        fprintf(stderr, "Unable to set playback sample rate: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    if (rate != SAMPLE_RATE) {
        fprintf(stderr, "Warning: Playback sample rate set to %u Hz instead of %u Hz\n", rate, SAMPLE_RATE);
    }
    rc = snd_pcm_hw_params_set_period_size_near(playback_handle, playback_params, &frames, &dir);
    if (rc < 0) {
        fprintf(stderr, "Unable to set playback period size: %s\n", snd_strerror(rc));
        goto cleanup;
    }
    rc = snd_pcm_hw_params(playback_handle, playback_params);
    if (rc < 0) {
        fprintf(stderr, "Unable to apply playback parameters: %s\n", snd_strerror(rc));
        goto cleanup;
    }

    // Open the WAV file for playback
    fp = fopen("output.wav", "rb");
    if (!fp) {
        perror("Failed to open WAV file for playback");
        goto cleanup;
    }

    // Skip WAV header (44 bytes)
    fseek(fp, 44, SEEK_SET);

    // Playback loop
    printf("Playing recorded audio...\n");
    while (1) {
        size_t size = fread(buffer, sizeof(int16_t), BUFFER_FRAMES * CHANNELS, fp);
        if (size == 0) {
            if (feof(fp)) break; // End of file
            fprintf(stderr, "Error reading WAV file\n");
            goto cleanup;
        }

        snd_pcm_uframes_t frames_to_write = size / CHANNELS; // Number of frames
        snd_pcm_uframes_t frames_written = 0;
        while (frames_written < frames_to_write) {
            snd_pcm_sframes_t ret = snd_pcm_writei(playback_handle, buffer + frames_written * CHANNELS, frames_to_write - frames_written);
            if (ret < 0) {
                if (ret == -EPIPE) {
                    rc = snd_pcm_prepare(playback_handle);
                    if (rc < 0) {
                        fprintf(stderr, "Failed to recover from playback underrun: %s\n", snd_strerror(rc));
                        goto cleanup;
                    }
                    continue;
                }
                fprintf(stderr, "Write error: %s\n", snd_strerror(ret));
                goto cleanup;
            }
            frames_written += ret;
        }
    }
    printf("Playback complete.\n");

cleanup:
    if (fp) fclose(fp);
    if (capture_handle) snd_pcm_close(capture_handle);
    if (playback_handle) {
        snd_pcm_drain(playback_handle);
        snd_pcm_close(playback_handle);
    }
    return rc < 0 ? 1 : 0;
}
