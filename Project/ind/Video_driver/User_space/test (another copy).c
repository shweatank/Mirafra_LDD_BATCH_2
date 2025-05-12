#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <time.h>

#define DEVICE "/dev/video0" // Change to /dev/video1 if needed
#define WIDTH 640
#define HEIGHT 480
#define NUM_BUFFERS 4

struct buffer {
    void *start;
    size_t length;
};

__u32 try_formats[] = {
    V4L2_PIX_FMT_YUYV,
    V4L2_PIX_FMT_MJPEG,
    V4L2_PIX_FMT_H264
};

const char *get_format_name(__u32 fmt) {
    switch (fmt) {
        case V4L2_PIX_FMT_YUYV: return "YUYV";
        case V4L2_PIX_FMT_MJPEG: return "MJPEG";
        case V4L2_PIX_FMT_H264: return "H264";
        default: return "UNKNOWN";
    }
}

const char *get_extension(__u32 fmt) {
    switch (fmt) {
        case V4L2_PIX_FMT_YUYV: return "yuv";
        case V4L2_PIX_FMT_MJPEG: return "mjpg";
        case V4L2_PIX_FMT_H264: return "h264";
        default: return "bin";
    }
}

int set_supported_format(int fd, __u32 *chosen_fmt) {
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    for (int i = 0; i < sizeof(try_formats)/sizeof(try_formats[0]); i++) {
        fmt.fmt.pix.pixelformat = try_formats[i];
        if (ioctl(fd, VIDIOC_S_FMT, &fmt) == 0) {
            *chosen_fmt = try_formats[i];
            printf("✅ Selected format: %s\n", get_format_name(*chosen_fmt));
            return 0;
        } else {
            fprintf(stderr, "Format %s not supported.\n", get_format_name(try_formats[i]));
        }
    }

    return -1;
}

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    __u32 pixel_format;
    if (set_supported_format(fd, &pixel_format) < 0) {
        fprintf(stderr, "❌ No supported pixel format found.\n");
        close(fd);
        return 1;
    }

    struct v4l2_requestbuffers req = {0};
    req.count = NUM_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        close(fd);
        return 1;
    }

    struct buffer *buffers = calloc(req.count, sizeof(*buffers));
    for (int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            return 1;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("mmap");
            return 1;
        }
    }

    for (int i = 0; i < req.count; ++i) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            return 1;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return 1;
    }

    char filename[64];
    snprintf(filename, sizeof(filename), "capture.%s", get_extension(pixel_format));
    FILE *video = fopen(filename, "wb");
    if (!video) {
        perror("fopen");
        return 1;
    }

    printf("🎥 Capturing for 10 seconds to file: %s\n", filename);
    time_t start_time = time(NULL);

    while (time(NULL) - start_time < 10) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("VIDIOC_DQBUF");
            break;
        }

        fwrite(buffers[buf.index].start, buf.bytesused, 1, video);

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            break;
        }
    }

    fclose(video);
    ioctl(fd, VIDIOC_STREAMOFF, &type);

    for (int i = 0; i < req.count; ++i) {
        munmap(buffers[i].start, buffers[i].length);
    }

    free(buffers);
    close(fd);

    printf("✅ Capture complete. Saved to %s\n", filename);
    printf("▶️ To play: ffplay %s\n", filename);
    return 0;
}

