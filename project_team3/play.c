// Compile with:
// gcc play_mp4_video.c -o play_mp4_video `pkg-config --cflags --libs libavformat libavcodec libswscale libavutil sdl2`
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <video.mp4>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

//    av_register_all();  // Initialize FFmpeg
    avformat_network_init();

    AVFormatContext *fmt_ctx = NULL;
    if (avformat_open_input(&fmt_ctx, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return 1;
    }

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Failed to retrieve stream info\n");
        return 1;
    }

    int video_stream_index = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        fprintf(stderr, "No video stream found\n");
        return 1;
    }

    AVCodecParameters *codec_par = fmt_ctx->streams[video_stream_index]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codec_par->codec_id);
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_par);
    avcodec_open2(codec_ctx, codec, NULL);

    struct SwsContext *sws_ctx = sws_getContext(
        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
        codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    AVFrame *frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);
    uint8_t *buffer = (uint8_t *)av_malloc(num_bytes * sizeof(uint8_t));
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, buffer, AV_PIX_FMT_RGB24,
                         codec_ctx->width, codec_ctx->height, 1);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("MP4 Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       codec_ctx->width, codec_ctx->height, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             codec_ctx->width, codec_ctx->height);

    AVPacket *pkt = av_packet_alloc();
    SDL_Event event;

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    sws_scale(sws_ctx, (const uint8_t * const *)frame->data,
                              frame->linesize, 0, codec_ctx->height,
                              rgb_frame->data, rgb_frame->linesize);

                    SDL_UpdateTexture(texture, NULL, rgb_frame->data[0], rgb_frame->linesize[0]);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(1000 / 30); // ~30fps

                    if (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                            goto cleanup;
                    }
                }
            }
        }
        av_packet_unref(pkt);
    }

cleanup:
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    av_free(buffer);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return 0;
}
