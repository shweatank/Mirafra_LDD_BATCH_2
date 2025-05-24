#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>   
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#define DEVICE "/dev/video0"
#define WIDTH 640
#define HEIGHT 480
#define NUM_FRAMES 100

struct buffer {
    void   *start;
    size_t length;
};

static void errno_exit(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (r == -1 && errno == EINTR);
    return r;
}

int main()
{
    int fd = open(DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (fd < 0)
        errno_exit("Cannot open device");

    // Set format: YUYV 640x480
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  // YUYV format from camera
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (xioctl(fd, VIDIOC_S_FMT, &fmt) < 0)
        errno_exit("Setting Pixel Format");

    // Request buffers
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4;  // 4 buffers
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, VIDIOC_REQBUFS, &req) < 0)
        errno_exit("Requesting Buffer");

    // Map buffers
    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
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

    // Queue buffers
    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0)
            errno_exit("Queue Buffer");
    }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) < 0)
        errno_exit("Start Capture");

    // Prepare swscale context for YUYV -> RGB24
    struct SwsContext *sws_ctx = sws_getContext(
        WIDTH, HEIGHT, AV_PIX_FMT_YUYV422,
        WIDTH, HEIGHT, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);

    if (!sws_ctx)
        errno_exit("Could not initialize swscale context");

    // Allocate frames
    AVFrame *src_frame = av_frame_alloc();
    AVFrame *dst_frame = av_frame_alloc();
    if (!src_frame || !dst_frame)
        errno_exit("Could not allocate AVFrames");

    int rgb_bufsize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, WIDTH, HEIGHT, 1);
    uint8_t *rgb_buffer = malloc(rgb_bufsize);
    if (!rgb_buffer)
        errno_exit("Could not allocate RGB buffer");

    av_image_fill_arrays(dst_frame->data, dst_frame->linesize, rgb_buffer, AV_PIX_FMT_RGB24, WIDTH, HEIGHT, 1);

    src_frame->format = AV_PIX_FMT_YUYV422;
    src_frame->width = WIDTH;
    src_frame->height = HEIGHT;

    dst_frame->format = AV_PIX_FMT_RGB24;
    dst_frame->width = WIDTH;
    dst_frame->height = HEIGHT;

    printf("Starting capture and conversion...\n");

    for (int i = 0; i < NUM_FRAMES; i++) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        // Wait for frame ready
        struct timeval tv = {0};
        tv.tv_sec = 2;  // 2 seconds timeout

        int r = select(fd + 1, &fds, NULL, NULL, &tv);
        if (r == -1)
            errno_exit("select");
        if (r == 0) {
            fprintf(stderr, "select timeout\n");
            exit(EXIT_FAILURE);
        }

        // Dequeue buffer
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd, VIDIOC_DQBUF, &buf) < 0)
            errno_exit("Retrieving Frame");

        // Setup src_frame data pointer and linesize
        src_frame->data[0] = buffers[buf.index].start;
        src_frame->linesize[0] = WIDTH * 2;  // YUYV is 2 bytes per pixel

        // Convert YUYV to RGB24
        sws_scale(sws_ctx,
            (const uint8_t * const*)src_frame->data,
            src_frame->linesize,
            0,
            HEIGHT,
            dst_frame->data,
            dst_frame->linesize);

        // Save to PPM file
        char filename[256];
        snprintf(filename, sizeof(filename), "frame%03d.ppm", i);

        FILE *fout = fopen(filename, "wb");
        if (!fout) {
            perror("Cannot open output file");
            exit(EXIT_FAILURE);
        }
        fprintf(fout, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
        fwrite(dst_frame->data[0], 1, WIDTH * HEIGHT * 3, fout);
        fclose(fout);

        printf("Saved %s\n", filename);

        // Re-queue buffer
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0)
            errno_exit("Requeue Buffer");
    }

    // Stop streaming
    if (xioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
        errno_exit("Stop Capture");

    // Cleanup
    for (int i = 0; i < req.count; i++)
        munmap(buffers[i].start, buffers[i].length);

    free(buffers);
    free(rgb_buffer);
    av_frame_free(&src_frame);
    av_frame_free(&dst_frame);
    sws_freeContext(sws_ctx);

    close(fd);
    printf("Capture finished\n");

    return 0;
}

