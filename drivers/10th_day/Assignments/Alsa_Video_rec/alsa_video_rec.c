#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <time.h>

#define VIDEO_DEVICE "/dev/video0"
#define OUTPUT_FILE "output.yuv"
#define RECORD_SECONDS 10
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FRAME_RATE 30

struct buffer {
    void *start;
    size_t length;
};

static int fd = -1;
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;

double get_time_seconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void open_device() {
    fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open video device");
        exit(EXIT_FAILURE);
    }
}

void init_device() {
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_streamparm parm;

    // Check device capabilities
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("Failed to query capabilities");
        exit(EXIT_FAILURE);
    }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "Device does not support video capture\n");
        exit(EXIT_FAILURE);
    }

    // Set video format (YUYV, 640x480)
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = FRAME_WIDTH;
    fmt.fmt.pix.height = FRAME_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Failed to set format");
        exit(EXIT_FAILURE);
    }

    // Set frame rate (30 fps)
    memset(&parm, 0, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.timeperframe.numerator = 1;
    parm.parm.capture.timeperframe.denominator = FRAME_RATE;
    if (ioctl(fd, VIDIOC_S_PARM, &parm) == -1) {
        perror("Failed to set frame rate");
        exit(EXIT_FAILURE);
    }

    // Request buffers
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Failed to request buffers");
        exit(EXIT_FAILURE);
    }
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory\n");
        exit(EXIT_FAILURE);
    }

    // Allocate buffers
    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
            perror("Failed to query buffer");
            exit(EXIT_FAILURE);
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                                        MAP_SHARED, fd, buf.m.offset);
        if (buffers[n_buffers].start == MAP_FAILED) {
            perror("Failed to mmap buffer");
            exit(EXIT_FAILURE);
        }
    }
}

void start_capturing() {
    for (unsigned int i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("Failed to queue buffer");
            exit(EXIT_FAILURE);
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        perror("Failed to start streaming");
        exit(EXIT_FAILURE);
    }
}

void stop_capturing() {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) == -1) {
        perror("Failed to stop streaming");
        exit(EXIT_FAILURE);
    }
}

void record_video() {
    FILE *out_file = fopen(OUTPUT_FILE, "wb");
    if (!out_file) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    double start_time = get_time_seconds();
    while (get_time_seconds() - start_time < RECORD_SECONDS) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        // Dequeue buffer
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            perror("Failed to dequeue buffer");
            exit(EXIT_FAILURE);
        }

        // Write frame to file
        fwrite(buffers[buf.index].start, 1, buf.bytesused, out_file);

        // Requeue buffer
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("Failed to requeue buffer");
            exit(EXIT_FAILURE);
        }
    }

    fclose(out_file);
}

void play_video() {
    char command[256];
    snprintf(command, sizeof(command), "ffplay -f rawvideo -pixel_format yuyv422 -video_size %dx%d -framerate %d %s",
             FRAME_WIDTH, FRAME_HEIGHT, FRAME_RATE, OUTPUT_FILE);
    printf("Playing video with command: %s\n", command);
    system(command);
}

void uninit_device() {
    for (unsigned int i = 0; i < n_buffers; ++i) {
        if (munmap(buffers[i].start, buffers[i].length) == -1) {
            perror("Failed to unmap buffer");
        }
    }
    free(buffers);
}

void close_device() {
    if (close(fd) == -1) {
        perror("Failed to close device");
    }
}

int main() {
    printf("Opening video device...\n");
    open_device();

    printf("Initializing device...\n");
    init_device();

    printf("Starting video capture...\n");
    start_capturing();

    printf("Recording for %d seconds...\n", RECORD_SECONDS);
    record_video();

    printf("Stopping video capture...\n");
    stop_capturing();

    printf("Playing recorded video...\n");
    play_video();

    printf("Cleaning up...\n");
    uninit_device();
    close_device();

    printf("Done.\n");
    return 0;
}
