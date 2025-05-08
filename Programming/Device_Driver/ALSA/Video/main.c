#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

int main(int argc, char *argv[]) {
    const char *filename = "test.mp4";

    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file.\n");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Could not find stream info.\n");
        return -1;
    }

    int videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        fprintf(stderr, "No video stream found.\n");
        return -1;
    }

    AVCodecParameters *codecpar = pFormatCtx->streams[videoStream]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(codecpar->codec_id);
    if (!pCodec) {
        fprintf(stderr, "Unsupported codec!\n");
        return -1;
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, codecpar);
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        fprintf(stderr, "Could not open codec.\n");
        return -1;
    }

    AVFrame *pFrame = av_frame_alloc();
    AVFrame *pFrameRGB = av_frame_alloc();

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
                                            pCodecCtx->height, 32);
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer,
                         AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);

    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width, pCodecCtx->height,
                                                AV_PIX_FMT_RGB24,
                                                SWS_BILINEAR, NULL, NULL, NULL);

    // SDL2 Initialization
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Simple Video Player",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          pCodecCtx->width, pCodecCtx->height, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             pCodecCtx->width, pCodecCtx->height);

    AVPacket packet;
    int response;

    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            response = avcodec_send_packet(pCodecCtx, &packet);
            if (response < 0) {
                fprintf(stderr, "Error while sending packet: %s\n", av_err2str(response));
                continue;
            }

            while (response >= 0) {
                response = avcodec_receive_frame(pCodecCtx, pFrame);
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
                    break;
                } else if (response < 0) {
                    fprintf(stderr, "Error while receiving frame: %s\n", av_err2str(response));
                    return -1;
                }

                sws_scale(sws_ctx, (uint8_t const *const *)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGB->data, pFrameRGB->linesize);

                SDL_UpdateTexture(texture, NULL, pFrameRGB->data[0],
                                  pFrameRGB->linesize[0]);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);

                SDL_Event event;
                SDL_PollEvent(&event);
                if (event.type == SDL_QUIT) {
                    av_packet_unref(&packet);
                    goto cleanup;
                }

                SDL_Delay(40); // ~25 FPS
            }
        }
        av_packet_unref(&packet);
    }

cleanup:
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    sws_freeContext(sws_ctx);
    av_free(buffer);
    av_frame_free(&pFrame);
    av_frame_free(&pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}

