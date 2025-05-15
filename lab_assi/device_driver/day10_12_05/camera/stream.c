#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

#define WIDTH 640
#define HEIGHT 480

struct buffer {
    void   *start;
    size_t length;
};

int main() {
    int fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        perror("Opening video device");
        return 1;
    }

    // Set format
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
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

    printf("Camera is now ON and streaming. Press Ctrl+C to stop.\n");

    // Keep camera on (streaming) indefinitely
    while (1) {
        sleep(1);  // Just wait, you can also poll or monitor the device
    }

    // Not reached unless you add signal handling
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    munmap(buffer.start, buffer.length);
    close(fd);
    return 0;
}

