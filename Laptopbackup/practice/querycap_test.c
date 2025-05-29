#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

int main() {
    const char *device = "/dev/video2"; // change this if needed
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("Failed to open video device");
        return 1;
    }

    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));

    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("VIDIOC_QUERYCAP failed");
        close(fd);
        return 1;
    }

    printf("Driver:      %s\n", cap.driver);
    printf("Card:        %s\n", cap.card);
    printf("Bus Info:    %s\n", cap.bus_info);
    printf("Version:     %u\n", cap.version);
    printf("Capabilities: 0x%08X\n", cap.capabilities);

    close(fd);
    return 0;
}

