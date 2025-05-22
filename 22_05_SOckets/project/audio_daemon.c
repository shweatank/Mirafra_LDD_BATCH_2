#include <stdio.h>
#include <alsa/asoundlib.h>

int play_wav(const char *filename)
{
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    FILE *fp;
    int rc;
    int size;
    char *buffer;
    unsigned int rate = 44100;
    int dir = 0;

    fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    // Skip WAV header (44 bytes)
    fseek(fp, 44, SEEK_SET);

    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "snd_pcm_open error: %s\n", snd_strerror(rc));
        fclose(fp);
        return rc;
    }

    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
    snd_pcm_hw_params(handle, params);
    snd_pcm_prepare(handle);

    size = 4096;
    buffer = malloc(size);
    if (!buffer) {
        fclose(fp);
        snd_pcm_close(handle);
        return -1;
    }

    while ((rc = fread(buffer, 1, size, fp)) > 0) {
        int frames = rc / 4; // 2 channels * 2 bytes
        rc = snd_pcm_writei(handle, buffer, frames);
        if (rc == -EPIPE) {
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "snd_pcm_writei error: %s\n", snd_strerror(rc));
        }
    }

    free(buffer);
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: audio_daemon <wavfile>\n");
        return 1;
    }

    return play_wav(argv[1]);
}

