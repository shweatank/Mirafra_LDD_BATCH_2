#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <SDL2/SDL.h>

#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480

struct buffer {
    void   *start;
    size_t length;
};

static void errno_exit(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (r == -1 && errno == EINTR);
    return r;
}

int main() {
    int fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0) errno_exit("Opening video device");

    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0)
        errno_exit("Setting Pixel Format");

    struct v4l2_requestbuffers req = {0};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0)
        errno_exit("Requesting Buffer");

    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QUERYBUF, &buf) < 0)
            errno_exit("Querying Buffer");

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED)
            errno_exit("mmap");
    }

    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0)
            errno_exit("Queue Buffer");
    }

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0)
        errno_exit("Start Capture");

    struct SwsContext *sws_ctx = sws_getContext(WIDTH, HEIGHT, AV_PIX_FMT_YUYV422, WIDTH, HEIGHT, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
    AVFrame *src = av_frame_alloc();
    AVFrame *dst = av_frame_alloc();
    uint8_t *rgb_data = malloc(WIDTH * HEIGHT * 3);
    av_image_fill_arrays(dst->data, dst->linesize, rgb_data, AV_PIX_FMT_RGB24, WIDTH, HEIGHT, 1);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("Live Camera", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    printf("Press [Esc] or close the window to quit.\n");

    int running = 1;
    while (running) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
        if (select(fd + 1, &fds, NULL, NULL, &tv) == -1)
            errno_exit("select");

        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0)
            errno_exit("Retrieving Frame");

        src->data[0] = buffers[buf.index].start;
        src->linesize[0] = WIDTH * 2;

        sws_scale(sws_ctx, (const uint8_t * const*)src->data, src->linesize, 0, HEIGHT, dst->data, dst->linesize);

        SDL_UpdateTexture(texture, NULL, dst->data[0], WIDTH * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0)
            errno_exit("Requeue Buffer");

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
                break;
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    for (int i = 0; i < req.count; i++)
        munmap(buffers[i].start, buffers[i].length);
    free(buffers);
    free(rgb_data);
    av_frame_free(&src);
    av_frame_free(&dst);
    sws_freeContext(sws_ctx);
    close(fd);

    return 0;
}

