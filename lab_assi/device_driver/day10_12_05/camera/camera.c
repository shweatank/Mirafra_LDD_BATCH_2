#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480

struct buffer {
    void *start;
    size_t length;
};

int main() {
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        perror("Cannot open video device");
        return 1;
    }

    // Set video format
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Setting format");
        close(fd);
        return 1;
    }

    // Request buffers
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Requesting buffer");
        close(fd);
        return 1;
    }

    // Query buffer
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
        perror("Querying buffer");
        close(fd);
        return 1;
    }

    // Memory map
    struct buffer buffer;
    buffer.length = buf.length;
    buffer.start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

    if (buffer.start == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // Queue the buffer
    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        perror("Queueing buffer");
        return 1;
    }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Start streaming");
        return 1;
    }

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Camera Preview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if (!window || !renderer || !texture) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        // Wait for a frame
        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("Retrieving frame");
            break;
        }

        SDL_UpdateTexture(texture, NULL, buffer.start, WIDTH * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Requeue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Requeue buffer");
            break;
        }

        // Exit on ESC or window close
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                running = 0;
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    ioctl(fd, VIDIOC_STREAMOFF, &type);
    munmap(buffer.start, buffer.length);
    close(fd);

    return 0;
}

