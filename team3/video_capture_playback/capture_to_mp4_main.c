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
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

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
    // Open video device
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

    // Initialize FFmpeg
    avformat_network_init();

    AVFormatContext *out_fmt_ctx = NULL;
    AVStream *video_st = NULL;
    AVCodecContext *codec_ctx = NULL;
    AVCodec *codec = NULL;

    avformat_alloc_output_context2(&out_fmt_ctx, NULL, NULL, "output.mp4");
    if (!out_fmt_ctx)
        errno_exit("Could not create output context");

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec)
        errno_exit("Codec not found");

    video_st = avformat_new_stream(out_fmt_ctx, NULL);
    if (!video_st)
        errno_exit("Failed allocating stream");

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx)
        errno_exit("Could not alloc an encoding context");

    codec_ctx->height = HEIGHT;
    codec_ctx->width = WIDTH;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    codec_ctx->time_base = (AVRational){1, 30};
    codec_ctx->framerate = (AVRational){30, 1};
    codec_ctx->gop_size = 12;
    codec_ctx->max_b_frames = 2;
    codec_ctx->bit_rate = 400000;

    if (out_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (avcodec_open2(codec_ctx, codec, NULL) < 0)
        errno_exit("Cannot open video encoder");

    if (avcodec_parameters_from_context(video_st->codecpar, codec_ctx) < 0)
        errno_exit("Failed to copy codec params");

    if (!(out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&out_fmt_ctx->pb, "output.mp4", AVIO_FLAG_WRITE) < 0)
            errno_exit("Could not open output file");
    }

    if (avformat_write_header(out_fmt_ctx, NULL) < 0)
        errno_exit("Error occurred when opening output file");

    // Prepare swscale context for YUYV -> YUV420P conversion
    struct SwsContext *sws_ctx = sws_getContext(
        WIDTH, HEIGHT, AV_PIX_FMT_YUYV422,
        WIDTH, HEIGHT, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, NULL, NULL, NULL);

    if (!sws_ctx)
        errno_exit("Could not initialize swscale context");

    AVFrame *src_frame = av_frame_alloc();
    AVFrame *dst_frame = av_frame_alloc();
    if (!src_frame || !dst_frame)
        errno_exit("Could not allocate AVFrames");

    dst_frame->format = AV_PIX_FMT_YUV420P;
    dst_frame->width = WIDTH;
    dst_frame->height = HEIGHT;

    if (av_frame_get_buffer(dst_frame, 32) < 0)
        errno_exit("Could not allocate destination frame buffer");

    src_frame->format = AV_PIX_FMT_YUYV422;
    src_frame->width = WIDTH;
    src_frame->height = HEIGHT;

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    printf("Starting capture and encode...\n");

    for (int i = 0; i < NUM_FRAMES; i++) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

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
        src_frame->linesize[0] = WIDTH * 2;  // YUYV422 is 2 bytes per pixel

        // Convert YUYV422 to YUV420P (dst_frame)
        sws_scale(sws_ctx,
                  (const uint8_t * const*)src_frame->data,
                  src_frame->linesize,
                  0,
                  HEIGHT,
                  dst_frame->data,
                  dst_frame->linesize);

        dst_frame->pts = i;

        // Send frame to encoder
        int ret = avcodec_send_frame(codec_ctx, dst_frame);
        if (ret < 0)
            errno_exit("Error sending a frame for encoding");

        // Receive packet(s)
        while (ret >= 0) {
            ret = avcodec_receive_packet(codec_ctx, &pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0)
                errno_exit("Error during encoding");

            // Rescale packet timestamp
            av_packet_rescale_ts(&pkt, codec_ctx->time_base, video_st->time_base);
            pkt.stream_index = video_st->index;

            // Write packet
            ret = av_interleaved_write_frame(out_fmt_ctx, &pkt);
            av_packet_unref(&pkt);
            if (ret < 0)
                errno_exit("Error while writing packet");
        }

        // Re-queue buffer
        if (xioctl(fd, VIDIOC_QBUF, &buf) < 0)
            errno_exit("Requeue Buffer");

        printf("Frame %d encoded and written\n", i);
    }

    // Flush encoder
    avcodec_send_frame(codec_ctx, NULL);
    while (1) {
        int ret = avcodec_receive_packet(codec_ctx, &pkt);
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
            break;
        else if (ret < 0)
            errno_exit("Error during flushing encoder");

        av_packet_rescale_ts(&pkt, codec_ctx->time_base, video_st->time_base);
        pkt.stream_index = video_st->index;

        ret = av_interleaved_write_frame(out_fmt_ctx, &pkt);
        av_packet_unref(&pkt);
        if (ret < 0)
            errno_exit("Error while writing flushed packet");
    }

    av_write_trailer(out_fmt_ctx);

    // Cleanup
    for (int i = 0; i < req.count; i++)
        munmap(buffers[i].start, buffers[i].length);

    free(buffers);

    av_frame_free(&src_frame);
    av_frame_free(&dst_frame);
    sws_freeContext(sws_ctx);

    avcodec_free_context(&codec_ctx);
    if (!(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&out_fmt_ctx->pb);
    avformat_free_context(out_fmt_ctx);

    close(fd);
    printf("Capture and encoding finished\n");

    return 0;
}

