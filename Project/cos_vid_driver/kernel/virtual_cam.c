/* virtual_cam.c - A virtual V4L2 camera kernel module with dynamic frame content */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/videodev2.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/jiffies.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-dev.h>

#define VIRTUAL_CAM_NAME "virtual-cam"
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define VIDEO_DEVICE_NUM 20 /* Register as /dev/video20 */
#define DEFAULT_FPS 30 /* Default frame rate */
#define MAX_BUFFERS 4 /* Support up to 4 buffers for MMAP */
#define BAR_WIDTH 20 /* Width of the moving bar in pixels */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mirafra");
MODULE_DESCRIPTION("Virtual V4L2 Camera Module with Dynamic Pattern");

struct virtual_cam_buffer {
    struct v4l2_buffer buf;
    bool in_queue;
    void *data; /* Buffer-specific memory */
    unsigned long offset; /* MMAP offset */
};

struct virtual_cam_device {
    struct v4l2_device v4l2_dev; /* V4L2 device */
    struct video_device *vdev;   /* Video device */
    struct v4l2_format fmt;
    struct mutex lock;
    u8 *frame_data;              /* Single frame data for read() */
    int current_input;           /* Current input index (default 0) */
    struct virtual_cam_buffer buffers[MAX_BUFFERS]; /* Multiple buffers for MMAP */
    int num_buffers;             /* Number of allocated buffers */
    bool streaming;              /* Streaming state */
    int bar_position;            /* Position of the moving bar */
};

static struct virtual_cam_device *vcam_dev;

/* Update the frame buffer with a moving vertical bar */
static void update_frame_buffer(void)
{
    int x, y;
    /* Clear the frame to gray (Y=128, UV=128) */
    memset(vcam_dev->frame_data, 128, FRAME_WIDTH * FRAME_HEIGHT * 2);

    /* Calculate bar position (moves left to right, then loops) */
    vcam_dev->bar_position = (vcam_dev->bar_position + 5) % (FRAME_WIDTH - BAR_WIDTH);

    /* Draw a white vertical bar (Y=255, UV=128 for white in YUYV) */
    for (y = 0; y < FRAME_HEIGHT; y++) {
        for (x = vcam_dev->bar_position; x < vcam_dev->bar_position + BAR_WIDTH; x += 2) {
            int idx = (y * FRAME_WIDTH + x) * 2;
            vcam_dev->frame_data[idx] = 255;     /* Y0 (white) */
            vcam_dev->frame_data[idx + 1] = 128; /* U */
            vcam_dev->frame_data[idx + 2] = 255; /* Y1 (white) */
            vcam_dev->frame_data[idx + 3] = 128; /* V */
        }
    }
}

static int virtual_cam_open(struct file *file)
{
    printk(KERN_INFO "Virtual cam: Device opened\n");
    file->private_data = vcam_dev;
    vcam_dev->bar_position = 0;
    return 0;
}

static int virtual_cam_release(struct file *file)
{
    printk(KERN_INFO "Virtual cam: Device released\n");
    return 0;
}

static ssize_t virtual_cam_read(struct file *file, char __user *buf,
                               size_t count, loff_t *ppos)
{
    size_t frame_size = FRAME_WIDTH * FRAME_HEIGHT * 2;

    /* Update frame content */
    update_frame_buffer();

    if (*ppos >= frame_size) {
        printk(KERN_INFO "Virtual cam: Read reached end of frame\n");
        return 0;
    }

    if (count > frame_size - *ppos)
        count = frame_size - *ppos;

    if (copy_to_user(buf, vcam_dev->frame_data + *ppos, count)) {
        printk(KERN_ERR "Virtual cam: Failed to copy data to user\n");
        return -EFAULT;
    }

    *ppos += count;
    printk(KERN_INFO "Virtual cam: Read %zu bytes\n", count);
    return count;
}

static int virtual_cam_querycap(struct file *file, void *priv,
                               struct v4l2_capability *cap)
{
    strscpy(cap->driver, VIRTUAL_CAM_NAME, sizeof(cap->driver));
    strscpy(cap->card, "Virtual Camera", sizeof(cap->card));
    strscpy(cap->bus_info, "platform:virtual-cam", sizeof(cap->bus_info));
    cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;
    cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
    return 0;
}

static int virtual_cam_enum_fmt(struct file *file, void *priv,
                               struct v4l2_fmtdesc *f)
{
    if (f->index > 0) {
        printk(KERN_DEBUG "Virtual cam: Invalid format index %d\n", f->index);
        return -EINVAL;
    }
    f->pixelformat = V4L2_PIX_FMT_YUYV;
    strscpy(f->description, "YUYV 4:2:2", sizeof(f->description));
    f->flags = 0;
    return 0;
}

static int virtual_cam_g_fmt(struct file *file, void *priv,
                            struct v4l2_format *f)
{
    memcpy(f, &vcam_dev->fmt, sizeof(*f));
    return 0;
}

static int virtual_cam_s_fmt(struct file *file, void *priv,
                            struct v4l2_format *f)
{
    if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        printk(KERN_DEBUG "Virtual cam: Unsupported pixel format %c%c%c%c\n",
               f->fmt.pix.pixelformat & 0xFF,
               (f->fmt.pix.pixelformat >> 8) & 0xFF,
               (f->fmt.pix.pixelformat >> 16) & 0xFF,
               (f->fmt.pix.pixelformat >> 24) & 0xFF);
        return -EINVAL;
    }

    if (f->fmt.pix.width != FRAME_WIDTH || f->fmt.pix.height != FRAME_HEIGHT) {
        printk(KERN_ERR "Virtual cam: Unsupported resolution %dx%d\n",
               f->fmt.pix.width, f->fmt.pix.height);
        return -EINVAL;
    }

    memcpy(&vcam_dev->fmt, f, sizeof(*f));
    printk(KERN_DEBUG "Virtual cam: Set format YUYV %dx%d\n",
           f->fmt.pix.width, f->fmt.pix.height);
    return 0;
}

static int virtual_cam_g_input(struct file *file, void *priv, unsigned int *i)
{
    *i = vcam_dev->current_input;
    printk(KERN_DEBUG "Virtual cam: Get input index %u\n", *i);
    return 0;
}

static int virtual_cam_s_input(struct file *file, void *priv, unsigned int i)
{
    if (i != 0) {
        printk(KERN_ERR "Virtual cam: Invalid input index %u\n", i);
        return -EINVAL;
    }
    vcam_dev->current_input = i;
    printk(KERN_DEBUG "Virtual cam: Set input index %u\n", i);
    return 0;
}

static int virtual_cam_enum_input(struct file *file, void *priv,
                                 struct v4l2_input *inp)
{
    if (inp->index != 0) {
        printk(KERN_DEBUG "Virtual cam: Invalid input index %u\n", inp->index);
        return -EINVAL;
    }
    strscpy(inp->name, "Virtual Input", sizeof(inp->name));
    inp->type = V4L2_INPUT_TYPE_CAMERA;
    inp->std = 0;
    inp->status = 0;
    inp->capabilities = 0;
    return 0;
}

static int virtual_cam_g_parm(struct file *file, void *priv,
                             struct v4l2_streamparm *p)
{
    if (p->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
        return -EINVAL;

    p->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    p->parm.capture.capturemode = 0;
    p->parm.capture.timeperframe.numerator = 1;
    p->parm.capture.timeperframe.denominator = DEFAULT_FPS;
    p->parm.capture.extendedmode = 0;
    p->parm.capture.readbuffers = 2;
    printk(KERN_DEBUG "Virtual cam: Get stream parameters, FPS=%d\n", DEFAULT_FPS);
    return 0;
}

static int virtual_cam_s_parm(struct file *file, void *priv,
                             struct v4l2_streamparm *p)
{
    if (p->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
        return -EINVAL;

    p->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    p->parm.capture.capturemode = 0;
    p->parm.capture.timeperframe.numerator = 1;
    p->parm.capture.timeperframe.denominator = DEFAULT_FPS;
    p->parm.capture.extendedmode = 0;
    p->parm.capture.readbuffers = 2;
    printk(KERN_DEBUG "Virtual cam: Set stream parameters, FPS=%d\n", DEFAULT_FPS);
    return 0;
}

static int virtual_cam_reqbufs(struct file *file, void *priv,
                              struct v4l2_requestbuffers *rb)
{
    int i;

    if (rb->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || rb->memory != V4L2_MEMORY_MMAP) {
        printk(KERN_DEBUG "Virtual cam: Unsupported buffer type %d or memory %d\n",
               rb->type, rb->memory);
        return -EINVAL;
    }

    /* Free existing buffers */
    for (i = 0; i < vcam_dev->num_buffers; i++) {
        if (vcam_dev->buffers[i].data) {
            vfree(vcam_dev->buffers[i].data);
            vcam_dev->buffers[i].data = NULL;
        }
    }
    vcam_dev->num_buffers = 0;

    /* Limit to MAX_BUFFERS */
    if (rb->count > MAX_BUFFERS)
        rb->count = MAX_BUFFERS;
    if (rb->count < 1) {
        rb->count = 0;
        printk(KERN_DEBUG "Virtual cam: Request buffers, count=0 (freed)\n");
        return 0;
    }

    /* Allocate new buffers */
    for (i = 0; i < rb->count; i++) {
        vcam_dev->buffers[i].data = vmalloc_user(FRAME_WIDTH * FRAME_HEIGHT * 2);
        if (!vcam_dev->buffers[i].data) {
            printk(KERN_ERR "Virtual cam: Failed to allocate buffer %d\n", i);
            /* Free allocated buffers */
            for (; i >= 0; i--) {
                vfree(vcam_dev->buffers[i].data);
                vcam_dev->buffers[i].data = NULL;
            }
            return -ENOMEM;
        }
        /* Initialize buffer */
        vcam_dev->buffers[i].buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vcam_dev->buffers[i].buf.memory = V4L2_MEMORY_MMAP;
        vcam_dev->buffers[i].buf.index = i;
        vcam_dev->buffers[i].buf.length = FRAME_WIDTH * FRAME_HEIGHT * 2;
        vcam_dev->buffers[i].in_queue = false;
        vcam_dev->buffers[i].offset = i * (FRAME_WIDTH * FRAME_HEIGHT * 2);
        /* Copy initial frame_data to buffer */
        memcpy(vcam_dev->buffers[i].data, vcam_dev->frame_data, FRAME_WIDTH * FRAME_HEIGHT * 2);
    }

    vcam_dev->num_buffers = rb->count;
    printk(KERN_DEBUG "Virtual cam: Request buffers, count=%u\n", rb->count);
    return 0;
}

static int virtual_cam_querybuf(struct file *file, void *priv,
                               struct v4l2_buffer *buf)
{
    if (buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || buf->memory != V4L2_MEMORY_MMAP ||
        buf->index >= vcam_dev->num_buffers) {
        printk(KERN_DEBUG "Virtual cam: Invalid buffer query: type=%d, memory=%d, index=%u\n",
               buf->type, buf->memory, buf->index);
        return -EINVAL;
    }

    buf->length = FRAME_WIDTH * FRAME_HEIGHT * 2;
    buf->m.offset = vcam_dev->buffers[buf->index].offset;
    buf->flags = V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
    printk(KERN_DEBUG "Virtual cam: Query buffer, index=%u, length=%u, offset=%lu\n",
           buf->index, buf->length, buf->m.offset);
    return 0;
}

static int virtual_cam_qbuf(struct file *file, void *priv,
                           struct v4l2_buffer *buf)
{
    if (buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || buf->memory != V4L2_MEMORY_MMAP ||
        buf->index >= vcam_dev->num_buffers) {
        printk(KERN_DEBUG "Virtual cam: Invalid queue buffer: type=%d, memory=%d, index=%u\n",
               buf->type, buf->memory, buf->index);
        return -EINVAL;
    }

    if (vcam_dev->buffers[buf->index].in_queue) {
        printk(KERN_DEBUG "Virtual cam: Buffer %u already queued\n", buf->index);
        return -EBUSY;
    }

    vcam_dev->buffers[buf->index].in_queue = true;
    buf->flags |= V4L2_BUF_FLAG_QUEUED;
    buf->flags &= ~V4L2_BUF_FLAG_DONE;
    printk(KERN_DEBUG "Virtual cam: Queue buffer, index=%u, flags=0x%x\n", buf->index, buf->flags);
    return 0;
}

static int virtual_cam_dqbuf(struct file *file, void *priv,
                            struct v4l2_buffer *buf)
{
    int i;

    if (buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || buf->memory != V4L2_MEMORY_MMAP ||
        buf->index >= vcam_dev->num_buffers) {
        printk(KERN_DEBUG "Virtual cam: Invalid dequeue buffer: type=%d, memory=%d, index=%u\n",
               buf->type, buf->memory, buf->index);
        return -EINVAL;
    }

    /* Find a queued buffer */
    for (i = 0; i < vcam_dev->num_buffers; i++) {
        if (vcam_dev->buffers[i].in_queue && vcam_dev->streaming) {
            /* Update frame content before dequeuing */
            update_frame_buffer();
            /* Copy updated frame_data to the buffer being dequeued */
            memcpy(vcam_dev->buffers[i].data, vcam_dev->frame_data, FRAME_WIDTH * FRAME_HEIGHT * 2);

            vcam_dev->buffers[i].in_queue = false;
            buf->index = i;
            buf->bytesused = FRAME_WIDTH * FRAME_HEIGHT * 2;
            buf->flags = V4L2_BUF_FLAG_DONE | V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
            buf->sequence = 0;
            ktime_t ts = ktime_get();
            buf->timestamp.tv_sec = ts / 1000000000;
            buf->timestamp.tv_usec = (ts % 1000000000) / 1000;
            printk(KERN_DEBUG "Virtual cam: Dequeue buffer, index=%u, bytesused=%u, flags=0x%x\n",
                   buf->index, buf->bytesused, buf->flags);
            return 0;
        }
    }

    printk(KERN_DEBUG "Virtual cam: No buffer queued or not streaming\n");
    return -EINVAL;
}

static int virtual_cam_streamon(struct file *file, void *priv, enum v4l2_buf_type type)
{
    if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        printk(KERN_DEBUG "Virtual cam: Invalid stream type %d\n", type);
        return -EINVAL;
    }
    vcam_dev->streaming = true;
    printk(KERN_DEBUG "Virtual cam: Stream on\n");
    return 0;
}

static int virtual_cam_streamoff(struct file *file, void *priv, enum v4l2_buf_type type)
{
    int i;

    if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        printk(KERN_DEBUG "Virtual cam: Invalid stream type %d\n", type);
        return -EINVAL;
    }
    vcam_dev->streaming = false;
    for (i = 0; i < vcam_dev->num_buffers; i++)
        vcam_dev->buffers[i].in_queue = false;
    printk(KERN_DEBUG "Virtual cam: Stream off\n");
    return 0;
}

static int virtual_cam_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long size = vma->vm_end - vma->vm_start;
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
    unsigned int index;

    /* Find the buffer corresponding to the offset */
    for (index = 0; index < vcam_dev->num_buffers; index++) {
        if (vcam_dev->buffers[index].offset == offset)
            break;
    }

    if (index >= vcam_dev->num_buffers || size > FRAME_WIDTH * FRAME_HEIGHT * 2) {
        printk(KERN_ERR "Virtual cam: Invalid MMAP size %lu or offset %lu\n", size, offset);
        return -EINVAL;
    }

    /* Map the buffer’s data */
    if (remap_vmalloc_range(vma, vcam_dev->buffers[index].data, 0)) {
        printk(KERN_ERR "Virtual cam: MMAP failed for buffer %u\n", index);
        return -ENOMEM;
    }

    printk(KERN_DEBUG "Virtual cam: MMAP successful, buffer=%u, size=%lu, addr=%p\n",
           index, size, vcam_dev->buffers[index].data);
    return 0;
}

static const struct v4l2_ioctl_ops virtual_cam_ioctl_ops = {
    .vidioc_querycap = virtual_cam_querycap,
    .vidioc_enum_fmt_vid_cap = virtual_cam_enum_fmt,
    .vidioc_g_fmt_vid_cap = virtual_cam_g_fmt,
    .vidioc_s_fmt_vid_cap = virtual_cam_s_fmt,
    .vidioc_g_input = virtual_cam_g_input,
    .vidioc_s_input = virtual_cam_s_input,
    .vidioc_enum_input = virtual_cam_enum_input,
    .vidioc_g_parm = virtual_cam_g_parm,
    .vidioc_s_parm = virtual_cam_s_parm,
    .vidioc_reqbufs = virtual_cam_reqbufs,
    .vidioc_querybuf = virtual_cam_querybuf,
    .vidioc_qbuf = virtual_cam_qbuf,
    .vidioc_dqbuf = virtual_cam_dqbuf,
    .vidioc_streamon = virtual_cam_streamon,
    .vidioc_streamoff = virtual_cam_streamoff,
};

static const struct v4l2_file_operations virtual_cam_fops = {
    .owner = THIS_MODULE,
    .open = virtual_cam_open,
    .release = virtual_cam_release,
    .read = virtual_cam_read,
    .unlocked_ioctl = video_ioctl2,
    .mmap = virtual_cam_mmap,
};

static int __init virtual_cam_init(void)
{
    int ret;

    printk(KERN_INFO "Virtual cam: Starting initialization\n");

    /* Allocate virtual camera device structure */
    vcam_dev = kzalloc(sizeof(*vcam_dev), GFP_KERNEL);
    if (!vcam_dev) {
        printk(KERN_ERR "Virtual cam: Failed to allocate vcam_dev\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "Virtual cam: vcam_dev allocated\n");

    /* Initialize V4L2 device */
    strscpy(vcam_dev->v4l2_dev.name, VIRTUAL_CAM_NAME, sizeof(vcam_dev->v4l2_dev.name));
    ret = v4l2_device_register(NULL, &vcam_dev->v4l2_dev);
    if (ret) {
        printk(KERN_ERR "Virtual cam: Failed to register v4l2_device, ret=%d\n", ret);
        kfree(vcam_dev);
        return ret;
    }
    printk(KERN_INFO "Virtual cam: v4l2_device registered\n");

    /* Initialize mutex */
    mutex_init(&vcam_dev->lock);
    printk(KERN_INFO "Virtual cam: Mutex initialized\n");

    /* Allocate frame buffer for read() */
    vcam_dev->frame_data = vmalloc_user(FRAME_WIDTH * FRAME_HEIGHT * 2);
    if (!vcam_dev->frame_data) {
        printk(KERN_ERR "Virtual cam: Failed to allocate frame_data\n");
        v4l2_device_unregister(&vcam_dev->v4l2_dev);
        kfree(vcam_dev);
        return -ENOMEM;
    }
    printk(KERN_INFO "Virtual cam: Frame buffer allocated\n");

    /* Initialize frame with a gray pattern (Y=128, UV=128 for YUYV) */
    memset(vcam_dev->frame_data, 128, FRAME_WIDTH * FRAME_HEIGHT * 2);
    printk(KERN_INFO "Virtual cam: Frame buffer initialized\n");

    /* Set up V4L2 format */
    vcam_dev->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vcam_dev->fmt.fmt.pix.width = FRAME_WIDTH;
    vcam_dev->fmt.fmt.pix.height = FRAME_HEIGHT;
    vcam_dev->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    vcam_dev->fmt.fmt.pix.field = V4L2_FIELD_NONE;
    vcam_dev->fmt.fmt.pix.bytesperline = FRAME_WIDTH * 2;
    vcam_dev->fmt.fmt.pix.sizeimage = FRAME_WIDTH * FRAME_HEIGHT * 2;
    printk(KERN_INFO "Virtual cam: V4L2 format set\n");

    /* Allocate video device */
    vcam_dev->vdev = video_device_alloc();
    if (!vcam_dev->vdev) {
        printk(KERN_ERR "Virtual cam: Failed to allocate video_device\n");
        vfree(vcam_dev->frame_data);
        v4l2_device_unregister(&vcam_dev->v4l2_dev);
        kfree(vcam_dev);
        return -ENOMEM;
    }
    printk(KERN_INFO "Virtual cam: video_device allocated\n");

    /* Initialize video device */
    strscpy(vcam_dev->vdev->name, VIRTUAL_CAM_NAME, sizeof(vcam_dev->vdev->name));
    vcam_dev->vdev->fops = &virtual_cam_fops;
    vcam_dev->vdev->ioctl_ops = &virtual_cam_ioctl_ops;
    vcam_dev->vdev->release = video_device_release;
    vcam_dev->vdev->v4l2_dev = &vcam_dev->v4l2_dev;
    vcam_dev->vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
    vcam_dev->vdev->vfl_dir = VFL_DIR_RX;
    vcam_dev->current_input = 0;
    vcam_dev->streaming = false;
    vcam_dev->num_buffers = 0;
    vcam_dev->bar_position = 0;
    printk(KERN_INFO "Virtual cam: video_device configured\n");

    /* Register video device as /dev/video20 */
    ret = video_register_device(vcam_dev->vdev, VFL_TYPE_VIDEO, VIDEO_DEVICE_NUM);
    if (ret) {
        printk(KERN_ERR "Virtual cam: Failed to register video device, ret=%d\n", ret);
        video_device_release(vcam_dev->vdev);
        vfree(vcam_dev->frame_data);
        v4l2_device_unregister(&vcam_dev->v4l2_dev);
        kfree(vcam_dev);
        return ret;
    }
    printk(KERN_INFO "Virtual cam: Registered as /dev/video%d\n", vcam_dev->vdev->num);
    return 0;
}

static void __exit virtual_cam_exit(void)
{
    int i;

    printk(KERN_INFO "Virtual cam: Unloading module\n");
    video_unregister_device(vcam_dev->vdev);
    vfree(vcam_dev->frame_data);
    for (i = 0; i < vcam_dev->num_buffers; i++)
        vfree(vcam_dev->buffers[i].data);
    v4l2_device_unregister(&vcam_dev->v4l2_dev);
    kfree(vcam_dev);
    printk(KERN_INFO "Virtual cam: Module unloaded\n");
}

module_init(virtual_cam_init);
module_exit(virtual_cam_exit);
