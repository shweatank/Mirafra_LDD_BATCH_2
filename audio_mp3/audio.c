#include <alsa/asoundlib.h>
#include <mpg123.h>

#define BUFFER_SIZE 4096

int main() {
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate;
    int dir;
    snd_pcm_uframes_t frames = 32;
    int rc;

    // Initialize mpg123
    mpg123_init();
    mpg123_handle *mpg = mpg123_new(NULL, NULL);
    mpg123_open(mpg, "test.mp3");
    mpg123_format_none(mpg);
    mpg123_format(mpg, 44100, MPG123_STEREO, MPG123_ENC_SIGNED_16);

    // Get MP3 file format details
    mpg123_getformat(mpg, &sample_rate, &dir, &rc); // rc stores encoding format

    // Open PCM device for playback
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
        exit(1);
    }

    // Allocate and set hardware parameters
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

    // Apply hardware parameters
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        exit(1);
    }

    // Buffer for decoded PCM data
    unsigned char buffer[BUFFER_SIZE];
    size_t size;

    // Playback loop: decode MP3 frames and send PCM data to ALSA
    while (mpg123_read(mpg, buffer, BUFFER_SIZE, &size) == MPG123_OK) {
        rc = snd_pcm_writei(handle, buffer, size / 4);
        if (rc == -EPIPE) { // Buffer underrun
            fprintf(stderr, "underrun occurred\n");
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
        }
    }

    // Cleanup
    mpg123_close(mpg);
    mpg123_delete(mpg);
    mpg123_exit();
    snd_pcm_drain(handle);
    snd_pcm_close(handle);

    return 0;
}
