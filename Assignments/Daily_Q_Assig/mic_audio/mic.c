#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define PCM_DEVICE "default"
#define CHANNELS 2
#define SAMPLE_RATE 20000
#define FORMAT SND_PCM_FORMAT_S16_LE
#define BITS_PER_SAMPLE 16
#define SECONDS 5
#define FRAME_SIZE (CHANNELS * (BITS_PER_SAMPLE / 8))
#define BUFFER_SIZE 4096

void write_wav_header(FILE *file, int sample_rate, short channels, short bits_per_sample, int data_size) {
    int byte_rate = sample_rate * channels * bits_per_sample / 8;
    short block_align = channels * bits_per_sample / 8;
    int chunk_size = 36 + data_size;

    fwrite("RIFF", 1, 4, file);
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);

    fwrite("fmt ", 1, 4, file);
    int subchunk1_size = 16;
    short audio_format = 1;
    fwrite(&subchunk1_size, 4, 1, file);
    fwrite(&audio_format, 2, 1, file);
    fwrite(&channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits_per_sample, 2, 1, file);

    fwrite("data", 1, 4, file);
    fwrite(&data_size, 4, 1, file);
}

int main() {
    snd_pcm_t *capture_handle = NULL, *playback_handle = NULL;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = SAMPLE_RATE;
    int dir;
    snd_pcm_uframes_t frames = 1024;
    int rc;

    char buffer[BUFFER_SIZE];
    int total_bytes = 0;

    FILE *wav_file = fopen("voice.wav", "wb+");
    if (!wav_file) {
        perror("Failed to open output file");
        return 1;
    }

    fseek(wav_file, 0, SEEK_SET); // Placeholder for WAV header

    // ---------- Recording ----------
    rc = snd_pcm_open(&capture_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open capture device: %s\n", snd_strerror(rc));
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(capture_handle, params);
    snd_pcm_hw_params_set_access(capture_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(capture_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(capture_handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(capture_handle, params, &frames, &dir);
    snd_pcm_hw_params(capture_handle, params);

    printf("Recording for %d seconds...\n", SECONDS);
    int bytes_to_record = SAMPLE_RATE * FRAME_SIZE * SECONDS;
    int remaining_bytes = bytes_to_record;

    struct pollfd pfd;
    snd_pcm_poll_descriptors(capture_handle, &pfd, 1);

    while (remaining_bytes > 0) {
        poll(&pfd, 1, 1000); // wait for up to 1 sec

        int frames_to_read = BUFFER_SIZE / FRAME_SIZE;
        int ret = snd_pcm_readi(capture_handle, buffer, frames_to_read);
        if (ret > 0) {
            int bytes = ret * FRAME_SIZE;
            if (bytes > remaining_bytes) bytes = remaining_bytes;
            fwrite(buffer, 1, bytes, wav_file);
            total_bytes += bytes;
            remaining_bytes -= bytes;
        } else if (ret == -EPIPE) {
            snd_pcm_prepare(capture_handle);
        } else {
            fprintf(stderr, "Read error: %s\n", snd_strerror(ret));
        }
    }

    snd_pcm_drain(capture_handle);
    snd_pcm_close(capture_handle);

    fseek(wav_file, 0, SEEK_SET);
    write_wav_header(wav_file, SAMPLE_RATE, CHANNELS, BITS_PER_SAMPLE, total_bytes);
    fclose(wav_file);
    printf("Recording done. File saved as voice.wav\n");

    // ---------- Playback ----------
    rc = snd_pcm_open(&playback_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open playback device: %s\n", snd_strerror(rc));
        return 1;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(playback_handle, params);
    snd_pcm_hw_params_set_access(playback_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(playback_handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate_near(playback_handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(playback_handle, params, &frames, &dir);
    snd_pcm_hw_params(playback_handle, params);

    wav_file = fopen("voice.wav", "rb");
    if (!wav_file) {
        perror("Failed to open WAV file for playback");
        return 1;
    }

    fseek(wav_file, 0, SEEK_SET); // skip header

    printf("Playing back recording...\n");
    while ((rc = fread(buffer, 1, BUFFER_SIZE, wav_file)) > 0) {
        int frames_to_write = rc / FRAME_SIZE;
        int ret = snd_pcm_writei(playback_handle, buffer, frames_to_write);
        if (ret < 0) {
            snd_pcm_prepare(playback_handle);
        }
    }

    fclose(wav_file);
    snd_pcm_drain(playback_handle);
    snd_pcm_close(playback_handle);

    printf("Playback finished.\n");
    return 0;
}

