#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-vmalloc.h>
#include <linux/kthread.h>
#include <linux/delay.h>


#define DRIVER_NAME "v4l2_capture"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("team3");
MODULE_DESCRIPTION("V4L2 Capture Driver");

// Device structures
static struct video_device *vdev;     // Represents /dev/videoX
static struct v4l2_device v4l2_dev;   //Core V4L2 device
static struct vb2_queue vb_queue;     // Video buffer queue
static struct mutex queue_lock;       //Lock for buffer queue operations

// Capture format 
#define WIDTH  640
#define HEIGHT 480
#define PIX_FMT V4L2_PIX_FMT_YUYV


struct v4l2_capture_dev {
    struct pci_dev *pdev;                // PCI device handle
    struct v4l2_device v4l2_dev;         // Core V4L2 device
    struct video_device vdev;           // Video device node (/dev/videoX)
    struct vb2_queue vb2_q;             // videobuf2 queue
    struct v4l2_pix_format format;      // Current capture format
    struct list_head queued_bufs;       // Internal buffer queue
    spinlock_t qlock;                   // Protects queued_bufs
    struct mutex lock;                  // Global ioctl lock
    unsigned int sequence;              // Frame counter
};

// Video buffer structure
struct v4l2_custom_buffer {
	struct vb2_buffer vb;
	struct list_head list;
	void *vaddr;
};

/* Queue setup: called when app requests buffers */
static int custom_queue_setup(struct vb2_queue *vq,
		unsigned int *nbuffers,
		unsigned int *nplanes,
		unsigned int sizes[],
		struct device *alloc_devs[])
{
	*nplanes = 1;
	sizes[0] = WIDTH * HEIGHT * 2; // Set Plane Size:
	return 0;
}

/* Called before a buffer is queued */
static int custom_buf_prepare(struct vb2_buffer *vb)
{ 
    
    struct v4l2_capture_dev *capdev = vb2_get_drv_priv(vb->vb2_queue);
    unsigned long size = capdev->format.sizeimage;
    
    /* Validate Buffer Size */

    if (vb2_plane_size(vb, 0) < size) {
        dev_err(&capdev->pdev->dev, "buffer too small (%lu < %lu)\n",
                vb2_plane_size(vb, 0), size);
        return -EINVAL;
    }

    vb2_set_plane_payload(vb, 0, size);
    vb->v4l2_buf.field = capdev->format.field;

	return 0;
}
/* Called when a buffer is queued by the user */
static void custom_buf_queue(struct vb2_buffer *vb)
{
	vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
}


static int custom_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	pr_info("Start streaming...\n");
	
	if (count < 2)
	  return -ENOBUFS;

	return 0;
}

static void custom_stop_streaming(struct vb2_queue *vq)
{
	
	pr_info("Stop streaming\n");

         /* stop */
	// Mark all remaining buffers as error
	while (!list_empty(&vq->queued_list)) {
		struct vb2_buffer *vb;
		vb = list_first_entry(&vq->queued_list, struct vb2_buffer, queued_entry);
		list_del(&vb->queued_entry);
		vb2_buffer_done(vb, VB2_BUF_STATE_ERROR);
	}
}

static const struct vb2_ops custom_vb2_ops = {
	.queue_setup     = custom_queue_setup,
	.buf_prepare     = custom_buf_prepare,
	.buf_queue       = custom_buf_queue,
	.start_streaming = custom_start_streaming,
	.stop_streaming  = custom_stop_streaming,
	.wait_prepare    = vb2_ops_wait_prepare,
	.wait_finish     = vb2_ops_wait_finish,
};

static int custom_vidioc_querycap(struct file *file, void *priv,
		struct v4l2_capability *cap)
{
	strscpy(cap->driver, DRIVER_NAME, sizeof(cap->driver));
	strscpy(cap->card, "Custom V4L2", sizeof(cap->card));
	strscpy(cap->bus_info, "platform:custom", sizeof(cap->bus_info));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	return 0;
}

static int custom_vidioc_fmt(struct file *file, void *priv,
		struct v4l2_format *f)
{
	f->fmt.pix.width       = WIDTH;
	f->fmt.pix.height      = HEIGHT;
	f->fmt.pix.pixelformat = PIX_FMT;
	f->fmt.pix.field       = V4L2_FIELD_NONE;
	f->fmt.pix.bytesperline = WIDTH * 2;
	f->fmt.pix.sizeimage   = WIDTH * HEIGHT * 2;
	return 0;
}

/* ioctl operations map for user-space control */
static const struct v4l2_ioctl_ops custom_ioctl_ops = {
	.vidioc_querycap      = custom_vidioc_querycap,//  check device capabilities
	.vidioc_g_fmt_vid_cap = custom_vidioc_fmt, //Get format
	.vidioc_s_fmt_vid_cap = custom_vidioc_fmt,//Set format
	.vidioc_reqbufs       = vb2_ioctl_reqbufs,//This allocates the buffers.
	.vidioc_querybuf      = vb2_ioctl_querybuf,//Returns size, address,
	.vidioc_qbuf          = vb2_ioctl_qbuf, //sends a buffer to the driver for filling.
	.vidioc_dqbuf         = vb2_ioctl_dqbuf,//takes a filled buffer back from the driver.
	.vidioc_streamon      = vb2_ioctl_streamon,//starts the video stream.
	.vidioc_streamoff     = vb2_ioctl_streamoff, //stops the video stream
};

static int custom_open(struct file *file)
{
	file->private_data = vdev;
	return 0;
}

static int custom_release(struct file *file)
{
	return 0;
}

static const struct v4l2_file_operations custom_fops = {
	.owner          = THIS_MODULE,
	.open           = custom_open,
	.release        = custom_release,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
	.poll           = vb2_fop_poll,
	.read           = vb2_fop_read,
};

static int __init v4l2_driver_init(void)
{
	int ret;

	pr_info("Initializing V4L2 capture driver...\n");

	/* Register V4L2 device core */
	ret = v4l2_device_register(NULL, &v4l2_dev);
	if (ret)
		return ret;

	/* Allocate video device structure */
	vdev = video_device_alloc();
	if (!vdev)
		return -ENOMEM;

	/* Setup video device  */
	strscpy(vdev->name, " V4L2 Device", sizeof(vdev->name));
	vdev->fops = &custom_fops;
	vdev->ioctl_ops = &custom_ioctl_ops;
	vdev->v4l2_dev = &v4l2_dev;
	vdev->release = video_device_release;

	mutex_init(&queue_lock);

	// Setup vb2 queue
	vb_queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //the type of buffer this queue handles
	vb_queue.io_modes = VB2_MMAP | VB2_READ; //how user-space applications can access the buffers
	vb_queue.ops = &custom_vb2_ops;  // Points to the set of callback functions that manage buffer operations.
	vb_queue.mem_ops = &vb2_vmalloc_memops; //Tells the kernel how to allocate memory for the buffers
	vb_queue.buf_struct_size = sizeof(struct v4l2_custom_buffer); //tells the framework how big each buffer structure should be
	vb_queue.lock = &queue_lock;// prevents race conditions
	vb_queue.drv_priv = vdev;//Private pointer for driver-specific data.

	/* Initialize queue */
	ret = vb2_queue_init(&vb_queue);
	if (ret) {
		video_device_release(vdev);
		return ret;
	}

	vdev->queue = &vb_queue;

	/* Register /dev/videoX   */
	ret = video_register_device(vdev, VFL_TYPE_VIDEO, -1);
	if (ret) {
		video_device_release(vdev);
		return ret;
	}

	pr_info("V4L2 driver registered as /dev/video%d\n", vdev->num);
	return 0;
}

static void __exit v4l2_driver_exit(void)
{
	video_unregister_device(vdev);
	v4l2_device_unregister(&v4l2_dev);
	pr_info("V4L2 driver unloaded\n");
}

/* Entry and exit points */
module_init(v4l2_driver_init);
module_exit(v4l2_driver_exit);

