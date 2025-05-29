#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/videodev2.h>
#include <linux/platform_device.h>
#include <media/videobuf2-vmalloc.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>

#define DEVICE_NAME "custom_v4l2"
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define FRAME_SIZE (FRAME_WIDTH * FRAME_HEIGHT * 2) // YUYV

struct custom_dev {
    struct video_device vdev;
    struct v4l2_device v4l2_dev;
    struct vb2_queue vb_queue;
    struct mutex lock;
    void *frame;
};

static int custom_queue_setup(struct vb2_queue *vq,
                              unsigned int *nbuffers,
                              unsigned int *nplanes,
                              unsigned int sizes[],
                              struct device *alloc_devs[])
{
    *nplanes = 1;
    sizes[0] = FRAME_SIZE;
    return 0;
}

static int custom_buffer_prepare(struct vb2_buffer *vb)
{
    if (vb2_plane_size(vb, 0) < FRAME_SIZE)
        return -EINVAL;
    vb2_set_plane_payload(vb, 0, FRAME_SIZE);
    return 0;
}

static void custom_buffer_queue(struct vb2_buffer *vb)
{
    struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
    memset(vb2_plane_vaddr(vb, 0), 0x80, FRAME_SIZE); // gray
    vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
}

static const struct vb2_ops custom_vb2_ops = {
    .queue_setup = custom_queue_setup,
    .buf_prepare = custom_buffer_prepare,
    .buf_queue = custom_buffer_queue,
    .mem_ops = &vb2_vmalloc_memops,
    .wait_prepare = vb2_ops_wait_prepare,
    .wait_finish = vb2_ops_wait_finish,
};

static int custom_querycap(struct file *file, void *priv, struct v4l2_capability *cap)
{
    strscpy(cap->driver, DEVICE_NAME, sizeof(cap->driver));
    strscpy(cap->card, "Custom V4L2", sizeof(cap->card));
    strscpy(cap->bus_info, "platform:custom", sizeof(cap->bus_info));
    cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
    return 0;
}

static int custom_fmt(struct file *file, void *priv, struct v4l2_format *f)
{
    f->fmt.pix.width = FRAME_WIDTH;
    f->fmt.pix.height = FRAME_HEIGHT;
    f->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    f->fmt.pix.field = V4L2_FIELD_NONE;
    f->fmt.pix.bytesperline = FRAME_WIDTH * 2;
    f->fmt.pix.sizeimage = FRAME_SIZE;
    return 0;
}

static const struct v4l2_ioctl_ops custom_ioctl_ops = {
    .vidioc_querycap = custom_querycap,
    .vidioc_g_fmt_vid_cap = custom_fmt,
    .vidioc_s_fmt_vid_cap = custom_fmt,
    .vidioc_reqbufs = vb2_ioctl_reqbufs,
    .vidioc_querybuf = vb2_ioctl_querybuf,
    .vidioc_qbuf = vb2_ioctl_qbuf,
    .vidioc_dqbuf = vb2_ioctl_dqbuf,
    .vidioc_streamon = vb2_ioctl_streamon,
    .vidioc_streamoff = vb2_ioctl_streamoff,
};

static const struct v4l2_file_operations custom_fops = {
    .owner = THIS_MODULE,
//    .open = vb2_fop_open,
    .release = vb2_fop_release,
    .mmap = vb2_fop_mmap,
    .poll = vb2_fop_poll,
    .unlocked_ioctl = video_ioctl2,
};

static struct custom_dev *dev;

static int __init custom_init(void)
{
    int ret;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    mutex_init(&dev->lock);
    strscpy(dev->v4l2_dev.name, DEVICE_NAME, sizeof(dev->v4l2_dev.name));

    ret = v4l2_device_register(NULL, &dev->v4l2_dev);
    if (ret)
        goto error_free;

    struct video_device *vdev = &dev->vdev;
    vdev->v4l2_dev = &dev->v4l2_dev;
    vdev->fops = &custom_fops;
    vdev->ioctl_ops = &custom_ioctl_ops;
    vdev->release = video_device_release_empty;
    vdev->lock = &dev->lock;
    strscpy(vdev->name, DEVICE_NAME, sizeof(vdev->name));

    struct vb2_queue *q = &dev->vb_queue;
    q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    q->io_modes = VB2_MMAP;
    q->drv_priv = dev;
    q->buf_struct_size = sizeof(struct vb2_v4l2_buffer);
    q->ops = &custom_vb2_ops;
    q->mem_ops = &vb2_vmalloc_memops;
    q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
    q->lock = &dev->lock;
    q->min_buffers_needed = 1;

    ret = vb2_queue_init(q);
    if (ret)
        goto error_v4l2;

    vdev->queue = q;

    ret = video_register_device(vdev, VFL_TYPE_VIDEO, -1);
    if (ret)
        goto error_v4l2;

    pr_info("custom v4l2 driver loaded as /dev/video%d\n", vdev->num);
    return 0;

error_v4l2:
    v4l2_device_unregister(&dev->v4l2_dev);
error_free:
    kfree(dev);
    return ret;
}

static void __exit custom_exit(void)
{
    video_unregister_device(&dev->vdev);
    v4l2_device_unregister(&dev->v4l2_dev);
    kfree(dev);
    pr_info("custom v4l2 driver unloaded\n");
}

module_init(custom_init);
module_exit(custom_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI GPT");
MODULE_DESCRIPTION("Minimal V4L2 Capture Driver with VB2");

