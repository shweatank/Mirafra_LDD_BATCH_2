#include <stdio.h>
#include <stdlib.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

#define SDL_AUDIO_BUFFER_SIZE 1024

// Global variables for SDL audio
uint8_t *audio_pos;
int audio_len;

// Audio callback for SDL
void audio_callback(void *userdata, Uint8 *stream, int len) {
    if (audio_len == 0) return;
    len = (len > audio_len ? audio_len : len);
    SDL_memcpy(stream, audio_pos, len);
    audio_pos += len;
    audio_len -= len;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <video_file>\n", argv[0]);
        return -1;
    }

    AVFormatContext *pFormatCtx = NULL;
    int videoStream = -1, audioStream = -1;

    avformat_network_init();
    if (avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0) {
        fprintf(stderr, "Failed to open video file\n");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) return -1;

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            videoStream = i;
        else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            audioStream = i;
    }

    if (videoStream == -1 || audioStream == -1) {
        fprintf(stderr, "Missing video or audio stream\n");
        return -1;
    }

    // Setup video codec
    AVCodecContext *videoCodecCtx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(videoCodecCtx, pFormatCtx->streams[videoStream]->codecpar);
    AVCodec *videoCodec = avcodec_find_decoder(videoCodecCtx->codec_id);
    avcodec_open2(videoCodecCtx, videoCodec, NULL);

    // Setup audio codec
    AVCodecContext *audioCodecCtx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(audioCodecCtx, pFormatCtx->streams[audioStream]->codecpar);
    AVCodec *audioCodec = avcodec_find_decoder(audioCodecCtx->codec_id);
    avcodec_open2(audioCodecCtx, audioCodec, NULL);

    // SDL setup
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    SDL_Window *win = SDL_CreateWindow("Video + Audio Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       videoCodecCtx->width, videoCodecCtx->height, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             videoCodecCtx->width, videoCodecCtx->height);

    AVFrame *frame = av_frame_alloc();
    AVFrame *frameRGB = av_frame_alloc();
    uint8_t *buffer = (uint8_t *) av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height, 1));
    av_image_fill_arrays(frameRGB->data, frameRGB->linesize, buffer, AV_PIX_FMT_RGB24,
                         videoCodecCtx->width, videoCodecCtx->height, 1);
    struct SwsContext *sws_ctx = sws_getContext(videoCodecCtx->width, videoCodecCtx->height,
                                                videoCodecCtx->pix_fmt, videoCodecCtx->width, videoCodecCtx->height,
                                                AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

    // Resampler setup
    SwrContext *swr = swr_alloc();
    av_opt_set_int(swr, "in_channel_layout", audioCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "out_channel_layout", audioCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", audioCodecCtx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", audioCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", audioCodecCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    swr_init(swr);

    // SDL audio
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = audioCodecCtx->sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = audioCodecCtx->channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
    wanted_spec.callback = audio_callback;

    if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
        fprintf(stderr, "SDL_OpenAudio error\n");
        return -1;
    }

    AVPacket pkt;
    SDL_Event event;
    SDL_PauseAudio(0);  // Start playing audio

    while (av_read_frame(pFormatCtx, &pkt) >= 0) {
        if (pkt.stream_index == videoStream) {
            if (avcodec_send_packet(videoCodecCtx, &pkt) == 0) {
                while (avcodec_receive_frame(videoCodecCtx, frame) == 0) {
                    sws_scale(sws_ctx, (uint8_t const * const *)frame->data, frame->linesize,
                              0, videoCodecCtx->height, frameRGB->data, frameRGB->linesize);
                    SDL_UpdateTexture(texture, NULL, frameRGB->data[0], frameRGB->linesize[0]);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(40);
                }
            }
        } else if (pkt.stream_index == audioStream) {
            if (avcodec_send_packet(audioCodecCtx, &pkt) == 0) {
                while (avcodec_receive_frame(audioCodecCtx, frame) == 0) {
                    uint8_t *out_buf[2] = { NULL };
                    int out_linesize;
                    int nb_samples = frame->nb_samples;
                    int out_size = av_samples_alloc_array_and_samples(out_buf, &out_linesize,
                                                                      audioCodecCtx->channels,
                                                                      nb_samples,
                                                                      AV_SAMPLE_FMT_S16, 0);
                    swr_convert(swr, out_buf, nb_samples,
                                (const uint8_t **)frame->data, nb_samples);

                    while (audio_len > 0) SDL_Delay(1);  // Wait for buffer to play

                    audio_pos = out_buf[0];
                    audio_len = nb_samples * audioCodecCtx->channels * 2;
                }
            }
        }

        av_packet_unref(&pkt);
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) break;
    }

    // Cleanup
    SDL_CloseAudio();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    swr_free(&swr);
    av_frame_free(&frame);
    av_frame_free(&frameRGB);
    avcodec_free_context(&videoCodecCtx);
    avcodec_free_context(&audioCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}

