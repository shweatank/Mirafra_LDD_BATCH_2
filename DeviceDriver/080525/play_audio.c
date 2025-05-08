#include <alsa/asoundlib.h>

int main() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = 44100;
    int dir;
    snd_pcm_uframes_t frames = 32;
    int rc;

    // Open PCM device for playback
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
        exit(1);
    }

    // Allocate hardware parameters object
    snd_pcm_hw_params_alloca(&params);

    // Fill with default values
    snd_pcm_hw_params_any(handle, params);

    // Set parameters
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2); // stereo
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    // Apply hardware parameters
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    // Open a WAV file
    FILE *fp = fopen("test.wav", "rb");
    if (!fp) {
        perror("Failed to open WAV file");
        return 1;
    }

    // Skip WAV header (44 bytes for PCM)
    fseek(fp, 44, SEEK_SET);

    // Buffer for audio data
    char buffer[4096];
    int size;

    // Playback loop
    while ((size = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        rc = snd_pcm_writei(handle, buffer, size / 4); // 2 channels * 16 bits = 4 bytes per frame
        if (rc == -EPIPE) {
            // Buffer underrun
            fprintf(stderr, "underrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
        }
    }

    // Cleanup
    fclose(fp);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    return 0;
}
