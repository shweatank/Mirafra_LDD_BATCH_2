#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#define SDL_AUDIO_BUFFER_SIZE 1024

// Audio buffer
uint8_t audio_buf[(192000 * 3) / 2];
unsigned int audio_buf_size = 0;
unsigned int audio_buf_index = 0;

AVCodecContext *audio_codec_ctx = NULL;
AVFormatContext *format_ctx = NULL;
int audio_stream_index = -1;
AVPacket *audio_pkt = NULL;
AVFrame *audio_frame = NULL;
SwrContext *swr_ctx = NULL;

void audio_callback(void *userdata, Uint8 *stream, int len) {
    while (len > 0) {
        if (audio_buf_index >= audio_buf_size) {
            int ret;
            audio_buf_size = 0;
            audio_buf_index = 0;
            while ((ret = av_read_frame(format_ctx, audio_pkt)) >= 0) {
                if (audio_pkt->stream_index == audio_stream_index) {
                    ret = avcodec_send_packet(audio_codec_ctx, audio_pkt);
                    if (ret < 0) {
                        fprintf(stderr, "Error sending audio packet: %d\n", ret);
                        continue;
                    }
                    ret = avcodec_receive_frame(audio_codec_ctx, audio_frame);
                    if (ret >= 0) {
                        uint8_t *out_buf[1] = { audio_buf };
                        audio_buf_size = swr_convert(swr_ctx, out_buf, sizeof(audio_buf) / 2,
                                                     (const uint8_t **)audio_frame->data, audio_frame->nb_samples);
                        if (audio_buf_size < 0) {
                            fprintf(stderr, "swr_convert failed: %d\n", audio_buf_size);
                            break;
                        }
                        audio_buf_size *= 2 * 2; // 2 bytes/sample, 2 channels
                        audio_buf_index = 0;
                        break;
                    }
                }
                av_packet_unref(audio_pkt);
            }
            if (ret < 0) {
                SDL_memset(stream, 0, len); // Fill with silence
                break;
            }
        }

        int to_copy = audio_buf_size - audio_buf_index;
        if (to_copy > len) to_copy = len;
        SDL_memcpy(stream, audio_buf + audio_buf_index, to_copy);
        len -= to_copy;
        stream += to_copy;
        audio_buf_index += to_copy;
    }
}

void cleanup() {
    if (audio_frame) av_frame_free(&audio_frame);
    if (audio_pkt) av_packet_free(&audio_pkt);
    if (swr_ctx) swr_free(&swr_ctx);
    if (audio_codec_ctx) avcodec_free_context(&audio_codec_ctx);
    if (format_ctx) avformat_close_input(&format_ctx);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <video_file>\n", argv[0]);
        return -1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (avformat_open_input(&format_ctx, argv[1], NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file\n");
        cleanup();
        return -1;
    }

    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream info\n");
        cleanup();
        return -1;
    }

    int video_stream_index = -1;
    AVCodecContext *video_codec_ctx = NULL;
    for (int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            audio_stream_index = i;
    }

    if (video_stream_index < 0) {
        fprintf(stderr, "No video stream found\n");
        cleanup();
        return -1;
    }

    // Video codec setup
    AVCodec *video_codec = avcodec_find_decoder(format_ctx->streams[video_stream_index]->codecpar->codec_id);
    if (!video_codec) {
        fprintf(stderr, "Video codec not found\n");
        cleanup();
        return -1;
    }
    video_codec_ctx = avcodec_alloc_context3(video_codec);
    if (!video_codec_ctx || avcodec_parameters_to_context(video_codec_ctx, format_ctx->streams[video_stream_index]->codecpar) < 0 ||
        avcodec_open2(video_codec_ctx, video_codec, NULL) < 0) {
        fprintf(stderr, "Failed to initialize video codec\n");
        cleanup();
        return -1;
    }

    // Audio codec setup
    if (audio_stream_index >= 0) {
        AVCodec *audio_codec = avcodec_find_decoder(format_ctx->streams[audio_stream_index]->codecpar->codec_id);
        if (!audio_codec) {
            fprintf(stderr, "Audio codec not found\n");
            cleanup();
            return -1;
        }
        audio_codec_ctx = avcodec_alloc_context3(audio_codec);
        if (!audio_codec_ctx || avcodec_parameters_to_context(audio_codec_ctx, format_ctx->streams[audio_stream_index]->codecpar) < 0 ||
            avcodec_open2(audio_codec_ctx, audio_codec, NULL) < 0) {
            fprintf(stderr, "Failed to initialize audio codec\n");
            cleanup();
            return -1;
        }

        audio_pkt = av_packet_alloc();
        audio_frame = av_frame_alloc();
        if (!audio_pkt || !audio_frame) {
            fprintf(stderr, "Failed to allocate audio packet/frame\n");
            cleanup();
            return -1;
        }

        // SwrContext setup
        swr_ctx = swr_alloc();
        if (!swr_ctx) {
            fprintf(stderr, "Failed to allocate SwrContext\n");
            cleanup();
            return -1;
        }
        av_opt_set_int(swr_ctx, "in_channel_layout", audio_codec_ctx->channel_layout, 0);
        av_opt_set_int(swr_ctx, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate", audio_codec_ctx->sample_rate, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate", 44100, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audio_codec_ctx->sample_fmt, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
        if (swr_init(swr_ctx) < 0) {
            fprintf(stderr, "Failed to initialize SwrContext\n");
            cleanup();
            return -1;
        }

        // SDL Audio setup
        SDL_AudioSpec wanted_spec = {
            .freq = 44100,
            .format = AUDIO_S16SYS,
            .channels = 2,
            .silence = 0,
            .samples = SDL_AUDIO_BUFFER_SIZE,
            .callback = audio_callback
        };
        SDL_AudioSpec obtained_spec;
        if (SDL_OpenAudio(&wanted_spec, &obtained_spec) < 0) {
            fprintf(stderr, "SDL_OpenAudio failed: %s\n", SDL_GetError());
            cleanup();
            return -1;
        }
        printf("Opened audio: freq=%d, channels=%d, format=%d\n",
               obtained_spec.freq, obtained_spec.channels, obtained_spec.format);
        SDL_PauseAudio(0);
    }

    // SDL Video setup
    AVFrame *frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    if (!frame || !rgb_frame) {
        fprintf(stderr, "Failed to allocate frames\n");
        cleanup();
        return -1;
    }

    struct SwsContext *sws_ctx = sws_getContext(
        video_codec_ctx->width, video_codec_ctx->height, video_codec_ctx->pix_fmt,
        video_codec_ctx->width, video_codec_ctx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        fprintf(stderr, "Failed to initialize SwsContext\n");
        cleanup();
        return -1;
    }

    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, video_codec_ctx->width, video_codec_ctx->height, 1);
    uint8_t *video_buffer = (uint8_t *)av_malloc(num_bytes);
    if (!video_buffer || av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, video_buffer, AV_PIX_FMT_RGB24,
                                              video_codec_ctx->width, video_codec_ctx->height, 1) < 0) {
        fprintf(stderr, "Failed to allocate video buffer\n");
        cleanup();
        return -1;
    }

    SDL_Window *win = SDL_CreateWindow("Video", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       video_codec_ctx->width, video_codec_ctx->height, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             video_codec_ctx->width, video_codec_ctx->height);
    if (!win || !renderer || !texture) {
        fprintf(stderr, "SDL video setup failed: %s\n", SDL_GetError());
        cleanup();
        return -1;
    }

    // Main loop
    AVPacket *packet = av_packet_alloc();
    if (!packet) {
        fprintf(stderr, "Failed to allocate packet\n");
        cleanup();
        return -1;
    }

    SDL_Event event;
    while (av_read_frame(format_ctx, packet) >= 0) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) break;

        if (packet->stream_index == video_stream_index) {
            if (avcodec_send_packet(video_codec_ctx, packet) == 0) {
                while (avcodec_receive_frame(video_codec_ctx, frame) == 0) {
                    sws_scale(sws_ctx, (const uint8_t *const *)frame->data, frame->linesize,
                              0, video_codec_ctx->height, rgb_frame->data, rgb_frame->linesize);
                    SDL_UpdateTexture(texture, NULL, rgb_frame->data[0], rgb_frame->linesize[0]);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    AVRational frame_rate = format_ctx->streams[video_stream_index]->avg_frame_rate;
                    if (frame_rate.num > 0 && frame_rate.den > 0) {
                        SDL_Delay(1000 * frame_rate.den / frame_rate.num);
                    }
                }
            }
        }
        av_packet_unref(packet);
    }

    // Cleanup
    SDL_CloseAudio();
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (win) SDL_DestroyWindow(win);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    av_packet_free(&packet);
    avcodec_free_context(&video_codec_ctx);
    sws_freeContext(sws_ctx);
    av_free(video_buffer);
    cleanup();
    return 0;
}
