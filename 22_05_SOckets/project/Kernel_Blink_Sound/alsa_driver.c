#include <linux/module.h>
#include <linux/kernel.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <linux/kthread.h>

#define SAMPLE_RATE 44100
#define BEEP_FREQ 440 // Hz
#define BEEP_DURATION_MS 200 // ms per beep
#define DOUBLE_BEEP_GAP_MS 100 // Gap between double beeps

static struct snd_card *card;
static struct snd_pcm *pcm;
static int sound_type = -1; // 0: single beep, 1: double beep
static struct task_struct *sound_thread = NULL;

// Generate sine wave for beep
static void generate_beep(s16 *buffer, int samples, int freq) {
    int i;
    for (i = 0; i < samples; i++) {
        buffer[i] = 32760 * sin(2 * M_PI * freq * i / SAMPLE_RATE);
    }
}

// Playback thread
static int sound_thread_fn(void *data) {
    struct snd_pcm_substream *substream;
    s16 *buffer;
    int samples = (BEEP_DURATION_MS * SAMPLE_RATE) / 1000;
    int ret;

    buffer = kmalloc(samples * sizeof(s16), GFP_KERNEL);
    if (!buffer)
        return -ENOMEM;

    generate_beep(buffer, samples, BEEP_FREQ);

    while (!kthread_should_stop()) {
        if (sound_type < 0) {
            msleep(10);
            continue;
        }

        // Open PCM substream
        ret = snd_pcm_kernel_open(pcm, 0, SNDRV_PCM_STREAM_PLAYBACK, &substream);
        if (ret < 0) {
            printk(KERN_ERR "PCM open failed: %d\n", ret);
            msleep(100);
            continue;
        }

        // Write single or double beep
        snd_pcm_kernel_write(substream, buffer, samples, SNDRV_PCM_FORMAT_S16_LE);
        if (sound_type == 1) {
            msleep(DOUBLE_BEEP_GAP_MS);
            snd_pcm_kernel_write(substream, buffer, samples, SNDRV_PCM_FORMAT_S16_LE);
        }

        snd_pcm_kernel_close(substream);
        sound_type = -1; // Reset
    }

    kfree(buffer);
    return 0;
}

// Exported function to play sound
void play_sound(int type) {
    sound_type = type;
}
EXPORT_SYMBOL(play_sound);

// PCM operations
static int custom_pcm_open(struct snd_pcm_substream *substream) {
    return 0;
}

static int custom_pcm_close(struct snd_pcm_substream *substream) {
    return 0;
}

static int custom_pcm_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params) {
    params->rate = SAMPLE_RATE;
    params->format = SNDRV_PCM_FORMAT_S16_LE;
    params->channels = 1;
    return 0;
}

static int custom_pcm_prepare(struct snd_pcm_substream *substream) {
    return 0;
}

static snd_pcm_uframes_t custom_pcm_pointer(struct snd_pcm_substream *substream) {
    return 0;
}

static int custom_pcm_trigger(struct snd_pcm_substream *substream, int cmd) {
    return 0;
}

static struct snd_pcm_ops custom_pcm_ops = {
    .open = custom_pcm_open,
    .close = custom_pcm_close,
    .hw_params = custom_pcm_hw_params,
    .prepare = custom_pcm_prepare,
    .pointer = custom_pcm_pointer,
    .trigger = custom_pcm_trigger,
};

static int __init alsa_driver_init(void) {
    int ret;

    // Create sound card
    ret = snd_card_new(&card, -1, "CustomALSA", THIS_MODULE, 0);
    if (ret < 0) {
        printk(KERN_ERR "Failed to create sound card: %d\n", ret);
        return ret;
    }

    // Create PCM device
    ret = snd_pcm_new(card, "CustomPCM", 0, 1, 0, &pcm);
    if (ret < 0) {
        snd_card_free(card);
        printk(KERN_ERR "Failed to create PCM: %d\n", ret);
        return ret;
    }

    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &custom_pcm_ops);

    // Register sound card
    ret = snd_card_register(card);
    if (ret < 0) {
        snd_card_free(card);
        printk(KERN_ERR "Failed to register sound card: %d\n", ret);
        return ret;
    }

    // Start sound thread
    sound_thread = kthread_run(sound_thread_fn, NULL, "sound_thread");
    if (IS_ERR(sound_thread)) {
        snd_card_free(card);
        printk(KERN_ERR "Failed to start sound thread\n");
        return PTR_ERR(sound_thread);
    }

    printk(KERN_INFO "ALSA driver initialized\n");
    return 0;
}

static void __exit alsa_driver_exit(void) {
    if (sound_thread)
        kthread_stop(sound_thread);
    if (card)
        snd_card_free(card);
    printk(KERN_INFO "ALSA driver exited\n");
}

module_init(alsa_driver_init);
module_exit(alsa_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom ALSA driver for beeps");