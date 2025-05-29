#include <alsa/asoundlib.h>

int main() {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = 44100;
    int dir;
    snd_pcm_uframes_t frames = 32;
    int rc;
    char *buffer;

    // Open PCM device for playback
    rc = snd_pcm_open(&pcm_handle, "hw:1,0", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Unable to open PCM device: %s\n", snd_strerror(rc));
        return 1;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);
    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, 2);  // stereo
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, &dir);
    snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &frames, &dir);
    snd_pcm_hw_params(pcm_handle, params);

    snd_pcm_hw_params_free(params);

    buffer = (char *) malloc(frames * 4);  // 2 bytes/sample * 2 channels

    FILE *f = fopen("audio.raw", "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    while (!feof(f)) {
        size_t n = fread(buffer, 1, frames * 4, f);
        if (n > 0)
            snd_pcm_writei(pcm_handle, buffer, frames);
    }

    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buffer);
    fclose(f);
    return 0;
}

