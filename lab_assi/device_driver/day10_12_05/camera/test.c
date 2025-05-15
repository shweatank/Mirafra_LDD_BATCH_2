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
    if (fd == -1) {
        perror("Opening video device");
        return 1;
    }

    // Set format for capturing video
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // YUYV format for webcam
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Setting Pixel Format");
        close(fd);
        return 1;
    }

    // Request buffer
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Requesting Buffer");
        close(fd);
        return 1;
    }

    // Query and mmap buffer
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
        perror("Querying Buffer");
        close(fd);
        return 1;
    }

    struct buffer buffer;
    buffer.length = buf.length;
    buffer.start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (buffer.start == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // Queue the buffer
    if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
        perror("Queue Buffer");
        close(fd);
        return 1;
    }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        perror("Start Streaming");
        close(fd);
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Init error: %s\n", SDL_GetError());
        close(fd);
        return 1;
    }

    // Create SDL window and renderer
    SDL_Window *window = SDL_CreateWindow("Camera Preview", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YUY2, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if (!window || !renderer || !texture) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        SDL_Quit();
        close(fd);
        return 1;
    }

    // Main loop for displaying the video stream
    SDL_Event event;
    int running = 1;

    while (running) {
        // Dequeue the buffer to get the frame
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            perror("Retrieving Frame");
            break;
        }

        // Update the SDL texture with the YUYV frame
        SDL_UpdateTexture(texture, NULL, buffer.start, WIDTH * 2); // YUYV format is 2 bytes per pixel
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Requeue the buffer to keep streaming
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("Requeueing Buffer");
            break;
        }

        // Check for SDL events (window close or ESC key)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        // Optionally, add a small sleep to avoid hogging the CPU
        usleep(10000);  // Sleep for 10 milliseconds
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Stop streaming and cleanup V4L2
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    munmap(buffer.start, buffer.length);
    close(fd);

    return 0;
}

