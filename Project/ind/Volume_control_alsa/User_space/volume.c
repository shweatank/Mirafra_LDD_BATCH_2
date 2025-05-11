#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/volctl"

void adjust_volume(int increment) {
    snd_mixer_t *mixer = NULL;
    snd_mixer_elem_t *elem = NULL;
    snd_mixer_selem_id_t *sid = NULL;
    long min, max, volume;
    int err;

    if ((err = snd_mixer_open(&mixer, 0)) < 0) {
        fprintf(stderr, "Failed to open mixer: %s\n", snd_strerror(err));
        return;
    }

    if ((err = snd_mixer_attach(mixer, "default")) < 0) {
        fprintf(stderr, "Failed to attach mixer to default card: %s\n", snd_strerror(err));
        snd_mixer_close(mixer);
        return;
    }

    if ((err = snd_mixer_selem_register(mixer, NULL, NULL)) < 0) {
        fprintf(stderr, "Failed to register mixer: %s\n", snd_strerror(err));
        snd_mixer_close(mixer);
        return;
    }

    if ((err = snd_mixer_load(mixer)) < 0) {
        fprintf(stderr, "Failed to load mixer: %s\n", snd_strerror(err));
        snd_mixer_close(mixer);
        return;
    }

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");

    elem = snd_mixer_find_selem(mixer, sid);
    if (!elem) {
        fprintf(stderr, "Unable to find Master control\n");
        snd_mixer_close(mixer);
        return;
    }

    if ((err = snd_mixer_selem_get_playback_volume_range(elem, &min, &max)) < 0) {
        fprintf(stderr, "Failed to get volume range: %s\n", snd_strerror(err));
        snd_mixer_close(mixer);
        return;
    }

    if ((err = snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &volume)) < 0) {
        fprintf(stderr, "Failed to get current volume: %s\n", snd_strerror(err));
        snd_mixer_close(mixer);
        return;
    }

    volume += increment * (max - min) / 20;
    if (volume < min) volume = min;
    if (volume > max) volume = max;

    if ((err = snd_mixer_selem_set_playback_volume_all(elem, volume)) < 0) {
        fprintf(stderr, "Failed to set volume: %s\n", snd_strerror(err));
    } else {
        printf("Volume set to %ld (range: %ld-%ld)\n", volume, min, max);
    }

    snd_mixer_close(mixer);
}

int main() {
    int fd;
    char buf[8];
    ssize_t len;

    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", DEVICE_PATH, strerror(errno));
        return 1;
    }

    printf("Listening for volume control events on %s...\n", DEVICE_PATH);

    while (1) {
        len = read(fd, buf, sizeof(buf) - 1);
        if (len < 0) {
            fprintf(stderr, "Read error: %s\n", strerror(errno));
            usleep(100000);
            continue;
        } else if (len == 0) {
            usleep(10000);
            continue;
        }

        buf[len] = '\0';
        printf("Received command: %s\n", buf);

        if (strcmp(buf, "UP") == 0) {
            adjust_volume(1);
        } else if (strcmp(buf, "DOWN") == 0) {
            adjust_volume(-1);
        } else {
            printf("Unknown command: %s\n", buf);
        }
    }

    close(fd);
    return 0;
}
