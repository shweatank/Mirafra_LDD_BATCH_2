#include<alsa/asoundlib.h>

int main()
{
   int rc; 
   int dir;
   int size;
   long loops;
   snd_pcm_t *handle;
   snd_pcm_hw_params_t  *params;
   snd_pcm_uframes_t frames;
   unsigned int val; 
//   char *buffer;


   /*open pcm device for PLAYBACK */
    rc = snd_pcm_open(&handle,"default",SND_PCM_STREAM_PLAYBACK,0);
    if(rc < 0){
        fprintf(stderr,"unable to open pcm device %s \n",snd_strerror(rc));
        exit(1);
     }
    /* allocate hardware parameters object */
    snd_pcm_hw_params_alloca(&params);
   
    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);
    
     /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle,params, SND_PCM_ACCESS_RW_INTERLEAVED);
 
    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_S16_LE); 
   /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params,2);
   /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;
  snd_pcm_hw_params_set_rate_near(handle, params,&val, &dir);

 /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);
    
  /* write paramters to the driver */
    rc = snd_pcm_hw_params(handle,params); 
    if(rc < 0){
     fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
      exit(1);
   }

 /* use buffer large enough to hold  one period */
   snd_pcm_hw_params_get_period_size(params,&frames,&dir); 
  size = frames * 4; /* 2bytes /sample , 2channels */
  char *buffer =  malloc(size);
      //char buffer[4096];
    /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                    &val, &dir);
  /* 5 seconds in microseconds divided by
   * period time */
  loops = 5000000 / val;
// Open a WAV file
	FILE *fp = fopen("test.wav", "rb");
	if (!fp) {
		perror("Failed to open WAV file");
		return 1;
	}

	// Skip WAV header (44 bytes for PCM)
	fseek(fp, 44, SEEK_SET);
   while((size = fread(buffer,1,size,fp)) > 0){
     rc = snd_pcm_writei(handle,buffer,frames);
     if (rc == -EPIPE) {
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr,
              "error from writei: %s\n",
              snd_strerror(rc));
    }  else if (rc != (int)size) {
      fprintf(stderr,
                   "short write, write %d frames\n", rc);
    }

   }
  fclose(fp);
  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);
  return 0;
} 
