#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <alsa/asoundlib.h>

#define MAX_FILES 1024
#define MAX_PATH 256
#define AUDIO_BUFFER_SIZE 4096

typedef struct {
    char *files[MAX_FILES];
    int count;
} FileList;

void read_mp4_files(FileList *list) {
    FILE *fp = fopen("/proc/mp4_list", "r");
    char line[MAX_PATH];
    list->count = 0;
    if (!fp) {
        fprintf(stderr, "Failed to open /proc/mp4_list: %s\n", strerror(errno));
        return;
    }
    while (fgets(line, MAX_PATH, fp) && list->count < MAX_FILES) {
        line[strcspn(line, "\n")] = 0;
        list->files[list->count] = strdup(line);
        if (!list->files[list->count]) {
            fprintf(stderr, "Failed to allocate memory for file path\n");
            continue;
        }
        list->count++;
    }
    fclose(fp);
}

void play_video(const char *filename, SDL_Window *window, SDL_Renderer *renderer) {
    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *video_ctx = NULL, *audio_ctx = NULL;
    AVStream *video_stream = NULL, *audio_stream = NULL;
    struct SwsContext *sws_ctx = NULL;
    SwrContext *swr_ctx = NULL;
    snd_pcm_t *pcm_handle = NULL;
    SDL_Texture *texture = NULL;
    int video_stream_idx = -1, audio_stream_idx = -1;
    int fullscreen = 0;

    // Initialize ALSA
    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        fprintf(stderr, "Failed to open ALSA device: %s\n", snd_strerror(errno));
        goto cleanup;
    }

    // Open video file
    if (avformat_open_input(&fmt_ctx, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Cannot open input file: %s\n", filename);
        goto cleanup;
    }
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Cannot find stream info for: %s\n", filename);
        goto cleanup;
    }

    // Find video and audio streams
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_stream_idx < 0) {
            video_stream_idx = i;
            video_stream = fmt_ctx->streams[i];
        } else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream_idx < 0) {
            audio_stream_idx = i;
            audio_stream = fmt_ctx->streams[i];
        }
    }

    if (video_stream_idx < 0) {
        fprintf(stderr, "No video stream found in: %s\n", filename);
        goto cleanup;
    }

    // Setup video codec
    const AVCodec *video_codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
    if (!video_codec) {
        fprintf(stderr, "Video codec not found for stream %d\n", video_stream_idx);
        goto cleanup;
    }
    video_ctx = avcodec_alloc_context3(video_codec);
    if (!video_ctx) {
        fprintf(stderr, "Failed to allocate video codec context\n");
        goto cleanup;
    }
    if (avcodec_parameters_to_context(video_ctx, video_stream->codecpar) < 0) {
        fprintf(stderr, "Failed to copy video codec parameters\n");
        goto cleanup;
    }
    if (avcodec_open2(video_ctx, video_codec, NULL) < 0) {
        fprintf(stderr, "Failed to open video codec\n");
        goto cleanup;
    }

    // Setup audio codec (if available)
    if (audio_stream_idx >= 0) {
        const AVCodec *audio_codec = avcodec_find_decoder(audio_stream->codecpar->codec_id);
        if (!audio_codec) {
            fprintf(stderr, "Audio codec not found for stream %d\n", audio_stream_idx);
            audio_stream_idx = -1; // Disable audio
        } else {
            audio_ctx = avcodec_alloc_context3(audio_codec);
            if (!audio_ctx) {
                fprintf(stderr, "Failed to allocate audio codec context\n");
                audio_stream_idx = -1;
            } else {
                if (avcodec_parameters_to_context(audio_ctx, audio_stream->codecpar) < 0) {
                    fprintf(stderr, "Failed to copy audio codec parameters\n");
                    avcodec_free_context(&audio_ctx);
                    audio_stream_idx = -1;
                } else if (avcodec_open2(audio_ctx, audio_codec, NULL) < 0) {
                    fprintf(stderr, "Failed to open audio codec\n");
                    avcodec_free_context(&audio_ctx);
                    audio_stream_idx = -1;
                } else {
                    // Configure ALSA
                    snd_pcm_hw_params_t *hw_params;
                    snd_pcm_hw_params_alloca(&hw_params);
                    snd_pcm_hw_params_any(pcm_handle, hw_params);
                    snd_pcm_hw_params_set_access(pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
                    snd_pcm_hw_params_set_format(pcm_handle, hw_params, SND_PCM_FORMAT_S16_LE);
                    unsigned int rate = audio_ctx->sample_rate;
                    snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &rate, 0);
                    snd_pcm_hw_params_set_channels(pcm_handle, hw_params, audio_ctx->channels);
                    if (snd_pcm_hw_params(pcm_handle, hw_params) < 0) {
                        fprintf(stderr, "Failed to set ALSA hardware parameters: %s\n", snd_strerror(errno));
                        avcodec_free_context(&audio_ctx);
                        audio_stream_idx = -1;
                    } else {
                        // Initialize swr for audio conversion
                        swr_ctx = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, audio_ctx->sample_rate,
                                                     av_get_default_channel_layout(audio_ctx->channels), audio_ctx->sample_fmt,
                                                     audio_ctx->sample_rate, 0, NULL);
                        if (!swr_ctx || swr_init(swr_ctx) < 0) {
                            fprintf(stderr, "Failed to initialize swr context\n");
                            swr_free(&swr_ctx);
                            avcodec_free_context(&audio_ctx);
                            audio_stream_idx = -1;
                        }
                    }
                }
            }
        }
    }

    // Setup SDL texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,
                               video_ctx->width, video_ctx->height);
    if (!texture) {
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());
        goto cleanup;
    }
    sws_ctx = sws_getContext(video_ctx->width, video_ctx->height, video_ctx->pix_fmt,
                             video_ctx->width, video_ctx->height, AV_PIX_FMT_YUV420P,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        fprintf(stderr, "Failed to initialize sws context\n");
        goto cleanup;
    }

    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    AVFrame *audio_frame = av_frame_alloc();
    if (!packet || !frame || !audio_frame) {
        fprintf(stderr, "Failed to allocate AV packet or frames\n");
        goto cleanup;
    }

    uint8_t *audio_buffer = av_malloc(AUDIO_BUFFER_SIZE);
    if (!audio_buffer) {
        fprintf(stderr, "Failed to allocate audio buffer\n");
        goto cleanup;
    }

    SDL_Event event;
    int quit = 0;
    while (!quit && av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == video_stream_idx) {
            if (avcodec_send_packet(video_ctx, packet) >= 0) {
                while (avcodec_receive_frame(video_ctx, frame) >= 0) {
                    AVFrame *yuv_frame = av_frame_alloc();
                    if (!yuv_frame) {
                        fprintf(stderr, "Failed to allocate YUV frame\n");
                        continue;
                    }
                    yuv_frame->format = AV_PIX_FMT_YUV420P;
                    yuv_frame->width = frame->width;
                    yuv_frame->height = frame->height;
                    if (av_frame_get_buffer(yuv_frame, 0) < 0) {
                        fprintf(stderr, "Failed to allocate YUV frame buffer\n");
                        av_frame_free(&yuv_frame);
                        continue;
                    }
                    sws_scale(sws_ctx, (const uint8_t *const *)frame->data, frame->linesize, 0, frame->height,
                              yuv_frame->data, yuv_frame->linesize);
                    SDL_UpdateYUVTexture(texture, NULL,
                                        yuv_frame->data[0], yuv_frame->linesize[0],
                                        yuv_frame->data[1], yuv_frame->linesize[1],
                                        yuv_frame->data[2], yuv_frame->linesize[2]);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    av_frame_free(&yuv_frame);
                }
            }
        } else if (packet->stream_index == audio_stream_idx && audio_ctx) {
            if (avcodec_send_packet(audio_ctx, packet) >= 0) {
                while (avcodec_receive_frame(audio_ctx, audio_frame) >= 0) {
                    uint8_t *out_buffer = audio_buffer;
                    int out_samples = swr_convert(swr_ctx, &out_buffer, AUDIO_BUFFER_SIZE / 2,
                                                 (const uint8_t **)audio_frame->data, audio_frame->nb_samples);
                    if (out_samples > 0) {
                        snd_pcm_writei(pcm_handle, audio_buffer, out_samples);
                    }
                }
            }
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_f && (event.key.keysym.mod & KMOD_ALT)) {
                    fullscreen = !fullscreen;
                    SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                }
            }
        }
        av_packet_unref(packet);
    }

cleanup:
    if (audio_buffer) av_free(audio_buffer);
    if (packet) av_packet_free(&packet);
    if (frame) av_frame_free(&frame);
    if (audio_frame) av_frame_free(&audio_frame);
    if (sws_ctx) sws_freeContext(sws_ctx);
    if (swr_ctx) swr_free(&swr_ctx);
    if (texture) SDL_DestroyTexture(texture);
    if (video_ctx) avcodec_free_context(&video_ctx);
    if (audio_ctx) avcodec_free_context(&audio_ctx);
    if (fmt_ctx) avformat_close_input(&fmt_ctx);
    if (pcm_handle) snd_pcm_close(pcm_handle);
}

int main() {
    FileList list = {0};
    read_mp4_files(&list);
    if (list.count == 0) {
        printf("No MP4 files found.\n");
        return 1;
    }

    printf("Found %d MP4 files:\n", list.count);
    for (int i = 0; i < list.count; i++) {
        printf("%d: %s\n", i + 1, list.files[i]);
    }

    int choice;
    printf("Enter the number of the video to play: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > list.count) {
        printf("Invalid choice.\n");
        for (int i = 0; i < list.count; i++)
            free(list.files[i]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        for (int i = 0; i < list.count; i++)
            free(list.files[i]);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Video Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        for (int i = 0; i < list.count; i++)
            free(list.files[i]);
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Failed to create SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        for (int i = 0; i < list.count; i++)
            free(list.files[i]);
        return 1;
    }

    play_video(list.files[choice - 1], window, renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    for (int i = 0; i < list.count; i++)
        free(list.files[i]);
    return 0;
}
