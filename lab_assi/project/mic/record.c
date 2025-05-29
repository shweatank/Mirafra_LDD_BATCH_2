#include <alsa/asoundlib.h>
#include <stdio.h>

#define PCM_DEVICE "default"

int main() {
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;
    unsigned int rate = 44100;
    int dir;
    int pcm;
    char *buffer;
    int frames = 32;

    pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_CAPTURE, 0);
    if (pcm < 0) {
        printf("ERROR: Can't open \"%s\" PCM device. %s\n", PCM_DEVICE, snd_strerror(pcm));
        return -1;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);

    snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, params, 2);
    snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, &dir);

    snd_pcm_hw_params(pcm_handle, params);

    snd_pcm_hw_params_get_period_size(params, (snd_pcm_uframes_t *)&frames, &dir);
    buffer = (char *) malloc(frames * 4); // 2 bytes/sample, 2 channels

    FILE *fp = fopen("recorded.wav", "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    printf("Recording... Press Ctrl+C to stop\n");
    for (int i = 0; i < 500; ++i) { // record ~5 seconds
        snd_pcm_readi(pcm_handle, buffer, frames);
        fwrite(buffer, 1, frames * 4, fp);
    }

    fclose(fp);
    snd_pcm_drain(pcm_handle);
    snd_pcm_close(pcm_handle);
    free(buffer);

    return 0;
}

