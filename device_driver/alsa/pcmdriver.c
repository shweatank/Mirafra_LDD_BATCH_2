#include <linux/module.h>
#include <linux/init.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>

#define CARD_NAME "Dummy ALSA Card"
#define PCM_NAME "Dummy PCM"

static struct snd_card *card;
static struct snd_pcm *pcm;

static int dummy_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	runtime->hw = (struct snd_pcm_hardware){
		.info = SNDRV_PCM_INFO_INTERLEAVED,
			.formats = SNDRV_PCM_FMTBIT_S16_LE,
			.rates = SNDRV_PCM_RATE_44100,
			.rate_min = 44100,
			.rate_max = 44100,
			.channels_min = 2,
			.channels_max = 2,
			.buffer_bytes_max = 64 * 1024,
			.period_bytes_min = 64,
			.period_bytes_max = 64 * 1024,
			.periods_min = 1,
			.periods_max = 1024,
	};
	return 0;
}

static int dummy_pcm_close(struct snd_pcm_substream *substream)
{
	return 0;
}

static int dummy_pcm_hw_params(struct snd_pcm_substream *substream,
		struct snd_pcm_hw_params *hw_params)
{
	return snd_pcm_lib_malloc_pages(substream,
			params_buffer_bytes(hw_params));
}

static int dummy_pcm_hw_free(struct snd_pcm_substream *substream)
{
	return snd_pcm_lib_free_pages(substream);
}

static int dummy_pcm_prepare(struct snd_pcm_substream *substream)
{
	return 0;
}

static int dummy_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	return 0;
}

static snd_pcm_uframes_t dummy_pcm_pointer(struct snd_pcm_substream *substream)
{
	return 0;
}

static struct snd_pcm_ops dummy_pcm_ops = {
	.open = dummy_pcm_open,
	.close = dummy_pcm_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = dummy_pcm_hw_params,
	.hw_free = dummy_pcm_hw_free,
	.prepare = dummy_pcm_prepare,
	.trigger = dummy_pcm_trigger,
	.pointer = dummy_pcm_pointer,
};

static int __init dummy_alsa_init(void)
{
	int err;

	err = snd_card_new(NULL, -1, NULL, THIS_MODULE, 0, &card);
	if (err < 0)
		return err;

	strcpy(card->driver, "dummy");
	strcpy(card->shortname, CARD_NAME);
	strcpy(card->longname, CARD_NAME);

	err = snd_pcm_new(card, PCM_NAME, 0, 1, 0, &pcm);
	if (err < 0)
		goto error;

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &dummy_pcm_ops);

	pcm->info_flags = 0;
	strcpy(pcm->name, PCM_NAME);

	err = snd_card_register(card);
	if (err < 0)
		goto error;

	pr_info("Dummy ALSA driver loaded\n");
	return 0;

error:
	snd_card_free(card);
	return err;
}

static void __exit dummy_alsa_exit(void)
{
	snd_card_free(card);
	pr_info("Dummy ALSA driver unloaded\n");
}

module_init(dummy_alsa_init);
devmodule_exit(dummy_alsa_exit);

MODULE_AUTHOR("Ganga");
MODULE_DESCRIPTION("Dummy ALSA PCM Driver");
MODULE_LICENSE("GPL");
