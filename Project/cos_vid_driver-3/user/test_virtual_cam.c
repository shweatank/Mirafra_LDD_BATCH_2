#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <errno.h>

#define VIDEO_DEVICE "/dev/video20"
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define OUTPUT_FILE "output.yuv"

int main() {
    int fd;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    char *buffer;
    ssize_t bytes_read;
    FILE *out_file;

    /* Open the video device */
    fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open video device");
        return 1;
    }
    printf("Opened %s successfully\n", VIDEO_DEVICE);

    /* Query device capabilities */
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("Failed to query capabilities");
        close(fd);
        return 1;
    }
    printf("Driver: %s\n", cap.driver);
    printf("Card: %s\n", cap.card);
    printf("Bus Info: %s\n", cap.bus_info);
    printf("Capabilities: 0x%08X\n", cap.capabilities);

    /* Set format */
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = FRAME_WIDTH;
    fmt.fmt.pix.height = FRAME_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("Failed to set format");
        close(fd);
        return 1;
    }
    printf("Format set: %dx%d, pixelformat: %c%c%c%c\n",
           fmt.fmt.pix.width, fmt.fmt.pix.height,
           fmt.fmt.pix.pixelformat & 0xFF,
           (fmt.fmt.pix.pixelformat >> 8) & 0xFF,
           (fmt.fmt.pix.pixelformat >> 16) & 0xFF,
           (fmt.fmt.pix.pixelformat >> 24) & 0xFF);

    /* Allocate buffer for one frame */
    buffer = malloc(FRAME_WIDTH * FRAME_HEIGHT * 2);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate buffer\n");
        close(fd);
        return 1;
    }

    /* Read one frame */
    bytes_read = read(fd, buffer, FRAME_WIDTH * FRAME_HEIGHT * 2);
    if (bytes_read < 0) {
        perror("Failed to read frame");
        free(buffer);
        close(fd);
        return 1;
    }
    printf("Read %zd bytes\n", bytes_read);

    /* Save frame to file */
    out_file = fopen(OUTPUT_FILE, "wb");
    if (!out_file) {
        perror("Failed to open output file");
        free(buffer);
        close(fd);
        return 1;
    }
    fwrite(buffer, 1, bytes_read, out_file);
    fclose(out_file);
    printf("Saved frame to %s\n", OUTPUT_FILE);

    /* Clean up */
    free(buffer);
    close(fd);
    return 0;
}
