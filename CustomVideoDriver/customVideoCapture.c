#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-vmalloc.h>
#include <linux/mutex.h>

#define DRIVER_NAME "v4l2_simple_capture"
#define WIDTH 640
#define HEIGHT 480
#define PIX_FMT V4L2_PIX_FMT_YUYV

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEAM3");
MODULE_DESCRIPTION("Simple V4L2 Video Capture Driver");

// Device structures
static struct v4l2_device v4l2_dev;   // Core V4L2 device
static struct video_device *vdev;      // Represents /dev/videoX
static struct vb2_queue vb_queue;      // Videobuf2 queue
static struct mutex queue_lock;        // Mutex for queue operations

// Custom buffer structure
struct simple_buffer {
    struct vb2_buffer vb;
    struct list_head list;
};

// Videobuf2 queue setup
static int queue_setup(struct vb2_queue *vq, unsigned int *nbuffers,
                       unsigned int *nplanes, unsigned int sizes[],
                       struct device *alloc_devs[])
{
    *nplanes = 1;                    // Single plane for YUYV
    sizes[0] = WIDTH * HEIGHT * 2;   // YUYV: 2 bytes per pixel
    if (*nbuffers == 0)
        *nbuffers = 1;               // Ensure at least one buffer
    return 0;
}

// Buffer preparation
static int buf_prepare(struct vb2_buffer *vb)
{
    if (vb2_plane_size(vb, 0) < WIDTH * HEIGHT * 2) {
        pr_err("Buffer too small\n");
        return -EINVAL;
    }
    vb2_set_plane_payload(vb, 0, WIDTH * HEIGHT * 2);
    return 0;
}

// Buffer queuing (placeholder)
static void buf_queue(struct vb2_buffer *vb)
{
    // Simulate frame capture by marking buffer as done
    vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
}

// Start streaming
static int start_streaming(struct vb2_queue *vq, unsigned int count)
{
    pr_info("Starting video stream\n");
    return 0;
}

// Stop streaming
static void stop_streaming(struct vb2_queue *vq)
{
    pr_info("Stopping video stream\n");
    // Clear queued buffers
    while (!list_empty(&vq->queued_list)) {  // Check if queue is not empty
        struct vb2_buffer *vb = list_first_entry(&vq->queued_list,
                                                 struct vb2_buffer, queued_entry);
        list_del(&vb->queued_entry); // Remove buffer from queue
        vb2_buffer_done(vb, VB2_BUF_STATE_ERROR); // Mark buffer as errored
    }
}

// Videobuf2 operations
static const struct vb2_ops vb2_ops = {
    .queue_setup     = queue_setup,
    .buf_prepare     = buf_prepare,
    .buf_queue       = buf_queue,
    .start_streaming = start_streaming,
    .stop_streaming  = stop_streaming,
};

// V4L2 capability query
static int vidioc_querycap(struct file *file, void *priv,
                           struct v4l2_capability *cap)
{
    strscpy(cap->driver, DRIVER_NAME, sizeof(cap->driver));
    strscpy(cap->card, "Simple V4L2 Capture", sizeof(cap->card));
    strscpy(cap->bus_info, "platform:simple", sizeof(cap->bus_info));
    cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
    return 0;
}

// Get/Set format
static int vidioc_fmt(struct file *file, void *priv, struct v4l2_format *f)
{
    f->fmt.pix.width = WIDTH;
    f->fmt.pix.height = HEIGHT;
    f->fmt.pix.pixelformat = PIX_FMT;
    f->fmt.pix.field = V4L2_FIELD_NONE;
    f->fmt.pix.bytesperline = WIDTH * 2;
    f->fmt.pix.sizeimage = WIDTH * HEIGHT * 2;
    return 0;
}

// V4L2 ioctl operations
static const struct v4l2_ioctl_ops ioctl_ops = {
    .vidioc_querycap      = vidioc_querycap,
    .vidioc_g_fmt_vid_cap = vidioc_fmt,
    .vidioc_s_fmt_vid_cap = vidioc_fmt,
    .vidioc_reqbufs       = vb2_ioctl_reqbufs,
    .vidioc_querybuf      = vb2_ioctl_querybuf,
    .vidioc_qbuf          = vb2_ioctl_qbuf,
    .vidioc_dqbuf         = vb2_ioctl_dqbuf,
    .vidioc_streamon      = vb2_ioctl_streamon,
    .vidioc_streamoff     = vb2_ioctl_streamoff,
};

// File operations
static const struct v4l2_file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = v4l2_fh_open,
    .release        = v4l2_fh_release,
    .unlocked_ioctl = video_ioctl2,
    .mmap           = vb2_fop_mmap,
    .poll           = vb2_fop_poll,
};

// Module initialization
static int __init simple_v4l2_init(void)
{
    int ret;

    pr_info("Initializing %s\n", DRIVER_NAME);

    // Register V4L2 device
    ret = v4l2_device_register(NULL, &v4l2_dev); // Register with no parent device
    if (ret) {
        pr_err("Failed to register V4L2 device\n");
        return ret;
    }

    // Allocate video device
    vdev = video_device_alloc();
    if (!vdev) {
        v4l2_device_unregister(&v4l2_dev);
        return -ENOMEM;
    }

    // Configure video device
    strscpy(vdev->name, DRIVER_NAME, sizeof(vdev->name));
    vdev->v4l2_dev = &v4l2_dev; // Link to V4L2 device
    vdev->fops = &fops;
    vdev->ioctl_ops = &ioctl_ops;
    vdev->release = video_device_release;

    // Initialize mutex
    mutex_init(&queue_lock);

    // Setup vb2 queue
	vb_queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //the type of buffer this queue handles
	vb_queue.io_modes = VB2_MMAP ; // Allow memory-mapped buffer access
	vb_queue.ops = &vb2_ops;  // Points to the set of callback functions that manage buffer operations.
	vb_queue.mem_ops = &vb2_vmalloc_memops; //Tells the kernel how to allocate memory for the buffers
	vb_queue.buf_struct_size = sizeof(struct simple_buffer); //tells the framework how big each buffer structure should be
	vb_queue.lock = &queue_lock;// prevents race conditions
	vb_queue.drv_priv = vdev;//Private pointer for driver-specific data.

    ret = vb2_queue_init(&vb_queue);
    if (ret) {
        video_device_release(vdev);
        v4l2_device_unregister(&v4l2_dev);
        return ret;
    }
    vdev->queue = &vb_queue;

    // Register video device
    ret = video_register_device(vdev, VFL_TYPE_VIDEO, -1); //Register /dev/videoX
    if (ret) {
        vb2_queue_release(&vb_queue);
        video_device_release(vdev);
        v4l2_device_unregister(&v4l2_dev);
        return ret;
    }

    pr_info("Registered as /dev/video%d\n", vdev->num);
    return 0;
}

// Module cleanup
static void __exit simple_v4l2_exit(void)
{
    video_unregister_device(vdev);
    vb2_queue_release(&vb_queue);
    v4l2_device_unregister(&v4l2_dev);
    pr_info("%s unloaded\n", DRIVER_NAME);
}

module_init(simple_v4l2_init);
module_exit(simple_v4l2_exit);
