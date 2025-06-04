#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_COUNT 10
#define DEVICE "/dev/video0"

int main() {
    // Open device
    int fd = open(DEVICE, O_RDWR);
    if (fd == -1) {
        perror("Failed to open device");
        return 1;
    }

    // Query capabilities
    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("Failed to query capabilities");
        close(fd);
        return 1;
    }
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "Device does not support video capture\n");
        close(fd);
        return 1;
    }
    printf("Driver: %s\n", cap.driver);

    // Set format (640x480, YUYV)
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Failed to set format");
        close(fd);
        return 1;
    }

    // Request one buffer (single buffer for simplicity)
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Failed to request buffers");
        close(fd);
        return 1;
    }

    // Query and map buffer
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
        perror("Failed to query buffer");
        close(fd);
        return 1;
    }
    void *buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (buffer == MAP_FAILED) {
        perror("Failed to mmap");
        close(fd);
        return 1;
    }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        perror("Failed to start streaming");
        munmap(buffer, buf.length);
        close(fd);
        return 1;
    }

    // Capture 100 frames
    for (int i = 0; i < FRAME_COUNT; i++) {
        // Queue buffer
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = 0;
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            perror("Failed to queue buffer");
            break;
        }

        // Dequeue buffer (wait for frame)
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            perror("Failed to dequeue buffer");
            break;
        }

        // Save frame to file
        char filename[32];
        snprintf(filename, sizeof(filename), "frame_%03d.yuv", i + 1);
        FILE *out = fopen(filename, "wb");
        if (out) {
            fwrite(buffer, buf.bytesused, 1, out);
            fclose(out);
            printf("Saved frame %d, size: %d bytes\n", i + 1, buf.bytesused);
        } else {
            perror("Failed to open output file");
        }
    }

    // Stop streaming
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) == -1) {
        perror("Failed to stop streaming");
    }

    // Clean up
    munmap(buffer, buf.length);
    close(fd);
    return 0;
}
