#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define AUDIO_BUFFER_SIZE 4096

typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *video_codec_ctx;
    AVCodecContext *audio_codec_ctx;
    int video_stream_idx;
    int audio_stream_idx;
    struct SwsContext *sws_ctx;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_AudioDeviceID audio_device;
    uint8_t *audio_buffer;
    int audio_buffer_size;
    int audio_buffer_index;
} PlayerContext;

// Supported video file extensions
const char *video_extensions[] = {".mp4", ".avi", ".mkv", ".mov", NULL};

void cleanup(PlayerContext *ctx) {
    if (ctx->audio_buffer) free(ctx->audio_buffer);
    if (ctx->audio_codec_ctx) avcodec_free_context(&ctx->audio_codec_ctx);
    if (ctx->video_codec_ctx) avcodec_free_context(&ctx->video_codec_ctx);
    if (ctx->fmt_ctx) avformat_close_input(&ctx->fmt_ctx);
    if (ctx->sws_ctx) sws_freeContext(ctx->sws_ctx);
    if (ctx->texture) SDL_DestroyTexture(ctx->texture);
    if (ctx->renderer) SDL_DestroyRenderer(ctx->renderer);
    if (ctx->window) SDL_DestroyWindow(ctx->window);
    if (ctx->audio_device) SDL_CloseAudioDevice(ctx->audio_device);
    SDL_Quit();
}

void audio_callback(void *userdata, Uint8 *stream, int len) {
    PlayerContext *ctx = (PlayerContext *)userdata;
    int len1;

    while (len > 0) {
        if (ctx->audio_buffer_index >= ctx->audio_buffer_size) {
            AVPacket pkt = {0};
            AVFrame *frame = av_frame_alloc();
            if (!frame) {
                fprintf(stderr, "Could not allocate frame\n");
                return;
            }

            if (av_read_frame(ctx->fmt_ctx, &pkt) >= 0) {
                if (pkt.stream_index == ctx->audio_stream_idx) {
                    if (avcodec_send_packet(ctx->audio_codec_ctx, &pkt) >= 0) {
                        if (avcodec_receive_frame(ctx->audio_codec_ctx, frame) >= 0) {
                            ctx->audio_buffer_size = frame->nb_samples * frame->channels * 2; // 16-bit audio
                            ctx->audio_buffer_index = 0;
                            memcpy(ctx->audio_buffer, frame->data[0], ctx->audio_buffer_size);
                        }
                    }
                }
                av_packet_unref(&pkt);
            } else {
                ctx->audio_buffer_size = 0;
                ctx->audio_buffer_index = 0;
                break;
            }
            av_frame_free(&frame);
        }

        len1 = ctx->audio_buffer_size - ctx->audio_buffer_index;
        if (len1 > len) len1 = len;
        memcpy(stream, ctx->audio_buffer + ctx->audio_buffer_index, len1);
        len -= len1;
        stream += len1;
        ctx->audio_buffer_index += len1;
    }
}

// Function to find a video file in the current directory
char *find_video_file() {
    DIR *dir = opendir(".");
    if (!dir) {
        fprintf(stderr, "Could not open current directory\n");
        return NULL;
    }

    struct dirent *entry;
    char *selected_file = NULL;

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) { // Regular file
            const char *ext = strrchr(entry->d_name, '.');
            if (ext) {
                for (int i = 0; video_extensions[i]; i++) {
                    if (strcasecmp(ext, video_extensions[i]) == 0) {
                        selected_file = strdup(entry->d_name);
                        break;
                    }
                }
            }
            if (selected_file) break;
        }
    }

    closedir(dir);
    return selected_file;
}

int main() {
    PlayerContext ctx = {0};
    ctx.audio_buffer = (uint8_t *)malloc(AUDIO_BUFFER_SIZE);
    if (!ctx.audio_buffer) {
        fprintf(stderr, "Could not allocate audio buffer\n");
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        cleanup(&ctx);
        return 1;
    }

    // Find a video file in the current directory
    char *video_file = find_video_file();
    if (!video_file) {
        fprintf(stderr, "No video file found in current directory\n");
        cleanup(&ctx);
        return 1;
    }
    printf("Playing video: %s\n", video_file);

    // Open video file
    if (avformat_open_input(&ctx.fmt_ctx, video_file, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open video file: %s\n", video_file);
        free(video_file);
        cleanup(&ctx);
        return 1;
    }
    free(video_file);

    if (avformat_find_stream_info(ctx.fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        cleanup(&ctx);
        return 1;
    }

    // Find video and audio streams
    ctx.video_stream_idx = -1;
    ctx.audio_stream_idx = -1;
    for (unsigned int i = 0; i < ctx.fmt_ctx->nb_streams; i++) {
        if (ctx.fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && ctx.video_stream_idx < 0) {
            ctx.video_stream_idx = i;
        }
        if (ctx.fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && ctx.audio_stream_idx < 0) {
            ctx.audio_stream_idx = i;
        }
    }

    if (ctx.video_stream_idx == -1) {
        fprintf(stderr, "No video stream found\n");
        cleanup(&ctx);
        return 1;
    }

    // Setup video codec
    AVCodec *video_codec = avcodec_find_decoder(ctx.fmt_ctx->streams[ctx.video_stream_idx]->codecpar->codec_id);
    ctx.video_codec_ctx = avcodec_alloc_context3(video_codec);
    if (avcodec_parameters_to_context(ctx.video_codec_ctx, ctx.fmt_ctx->streams[ctx.video_stream_idx]->codecpar) < 0) {
        fprintf(stderr, "Could not copy video codec parameters\n");
        cleanup(&ctx);
        return 1;
    }
    if (avcodec_open2(ctx.video_codec_ctx, video_codec, NULL) < 0) {
        fprintf(stderr, "Could not open video codec\n");
        cleanup(&ctx);
        return 1;
    }

    // Setup audio codec
    if (ctx.audio_stream_idx >= 0) {
        AVCodec *audio_codec = avcodec_find_decoder(ctx.fmt_ctx->streams[ctx.audio_stream_idx]->codecpar->codec_id);
        ctx.audio_codec_ctx = avcodec_alloc_context3(audio_codec);
        if (avcodec_parameters_to_context(ctx.audio_codec_ctx, ctx.fmt_ctx->streams[ctx.audio_stream_idx]->codecpar) < 0) {
            fprintf(stderr, "Could not copy audio codec parameters\n");
            cleanup(&ctx);
            return 1;
        }
        if (avcodec_open2(ctx.audio_codec_ctx, audio_codec, NULL) < 0) {
            fprintf(stderr, "Could not open audio codec\n");
            cleanup(&ctx);
            return 1;
        }
    }

    // Create SDL window in full-screen mode
    ctx.window = SDL_CreateWindow("Video Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!ctx.window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        cleanup(&ctx);
        return 1;
    }

    ctx.renderer = SDL_CreateRenderer(ctx.window, -1, SDL_RENDERER_ACCELERATED);
    if (!ctx.renderer) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        cleanup(&ctx);
        return 1;
    }

    // Get screen dimensions
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int screen_width = display_mode.w;
    int screen_height = display_mode.h;

    // Create texture for video
    ctx.texture = SDL_CreateTexture(ctx.renderer, SDL_PIXELFORMAT_YV12,
                                   SDL_TEXTUREACCESS_STREAMING, ctx.video_codec_ctx->width, ctx.video_codec_ctx->height);
    if (!ctx.texture) {
        fprintf(stderr, "Could not create texture: %s\n", SDL_GetError());
        cleanup(&ctx);
        return 1;
    }

    // Setup audio
    if (ctx.audio_stream_idx >= 0) {
        SDL_AudioSpec wanted_spec = {0}, spec;
        wanted_spec.freq = ctx.audio_codec_ctx->sample_rate;
        wanted_spec.format = AUDIO_S16SYS;
        wanted_spec.channels = ctx.audio_codec_ctx->channels;
        wanted_spec.silence = 0;
        wanted_spec.samples = 1024;
        wanted_spec.callback = audio_callback;
        wanted_spec.userdata = &ctx;

        ctx.audio_device = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, 0);
        if (ctx.audio_device == 0) {
            fprintf(stderr, "Could not open audio device: %s\n", SDL_GetError());
            cleanup(&ctx);
            return 1;
        }
        SDL_PauseAudioDevice(ctx.audio_device, 0);
    }

    // Setup swscale context
    ctx.sws_ctx = sws_getContext(ctx.video_codec_ctx->width, ctx.video_codec_ctx->height, ctx.video_codec_ctx->pix_fmt,
                                 ctx.video_codec_ctx->width, ctx.video_codec_ctx->height, AV_PIX_FMT_YUV420P,
                                 SWS_BILINEAR, NULL, NULL, NULL);
    if (!ctx.sws_ctx) {
        fprintf(stderr, "Could not initialize swscale context\n");
        cleanup(&ctx);
        return 1;
    }

    // Main playback loop
    AVPacket pkt = {0};
    AVFrame *frame = av_frame_alloc();
    AVFrame *frame_yuv = av_frame_alloc();
    if (!frame || !frame_yuv) {
        fprintf(stderr, "Could not allocate frames\n");
        cleanup(&ctx);
        av_frame_free(&frame);
        av_frame_free(&frame_yuv);
        return 1;
    }

    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                quit = 1;
                break;
            }
        }

        if (av_read_frame(ctx.fmt_ctx, &pkt) < 0) {
            break; // End of file
        }

        if (pkt.stream_index == ctx.video_stream_idx) {
            if (avcodec_send_packet(ctx.video_codec_ctx, &pkt) >= 0) {
                if (avcodec_receive_frame(ctx.video_codec_ctx, frame) >= 0) {
                    // Convert to YUV420P
                    frame_yuv->format = AV_PIX_FMT_YUV420P;
                    frame_yuv->width = frame->width;
                    frame_yuv->height = frame->height;
                    av_frame_get_buffer(frame_yuv, 0);
                    sws_scale(ctx.sws_ctx, (const uint8_t *const *)frame->data, frame->linesize, 0,
                              frame->height, frame_yuv->data, frame_yuv->linesize);

                    // Calculate scaling to maintain aspect ratio
                    float aspect_ratio = (float)frame->width / frame->height;
                    int dst_width, dst_height;
                    if (screen_width / aspect_ratio <= screen_height) {
                        dst_width = screen_width;
                        dst_height = (int)(screen_width / aspect_ratio);
                    } else {
                        dst_height = screen_height;
                        dst_width = (int)(screen_height * aspect_ratio);
                    }

                    // Center the video
                    SDL_Rect dst_rect = {
                        (screen_width - dst_width) / 2,
                        (screen_height - dst_height) / 2,
                        dst_width,
                        dst_height
                    };

                    // Update SDL texture
                    SDL_UpdateYUVTexture(ctx.texture, NULL,
                                        frame_yuv->data[0], frame_yuv->linesize[0],
                                        frame_yuv->data[1], frame_yuv->linesize[1],
                                        frame_yuv->data[2], frame_yuv->linesize[2]);
                    SDL_RenderClear(ctx.renderer);
                    SDL_RenderCopy(ctx.renderer, ctx.texture, NULL, &dst_rect);
                    SDL_RenderPresent(ctx.renderer);
                }
            }
        }
        av_packet_unref(&pkt);
    }

    // Cleanup
    av_frame_free(&frame);
    av_frame_free(&frame_yuv);
    cleanup(&ctx);
    return 0;
}
