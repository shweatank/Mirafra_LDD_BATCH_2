#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <media/v4l2-device.h>
#include <linux/string.h>
#include <media/v4l2-ioctl.h>
#include <linux/videodev2.h>
#include <media/videobuf2-vmalloc.h>
#include <linux/vmalloc.h>



#define WIDTH  640
#define HEIGHT 480
#define PIXEL_FORMAT V4L2_PIX_FMT_YUYV
#define FRAME_SIZE (WIDTH * HEIGHT * 2)

static void *frame_data;  // pointer to one fake frame
static struct vb2_queue vb_queue;

struct custom_buffer {
	struct vb2_v4l2_buffer vb;
	struct list_head list;
};

struct custom_dev {
    struct v4l2_device v4l2_dev;
    struct video_device *vdev;
    struct vb2_queue vb_queue;
    struct mutex lock;
    // Add more fields as needed
};

//static struct list_head buffer_list;
//static spinlock_t lock;
static struct mutex queue_lock;
struct custom_dev *dev;

static struct video_device *vdev;
static struct v4l2_device v4l2_dev;

struct my_stream {
	struct video_device *vdev;
	struct v4l2_device v4l2_dev;
	struct v4l2_format fmt;
	struct mutex lock;
};

//vb2 Queue Operations
static int queue_setup(struct vb2_queue *q, unsigned int *nbuffers,
		unsigned int *nplanes, unsigned int sizes[],
		struct device *alloc_devs[])
{
	*nbuffers = 1;
	*nplanes = 1;
	sizes[0] = FRAME_SIZE;
	return 0;
}

static int buffer_prepare(struct vb2_buffer *vb)
{
	vb2_set_plane_payload(vb, 0, FRAME_SIZE);
	return 0;
}

static void buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	void *vaddr = vb2_plane_vaddr(vb, 0);

	memcpy(vaddr, frame_data, FRAME_SIZE);
	vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
}

static int custom_enum_fmt_vid_cap(struct file *file, void *priv, struct v4l2_fmtdesc *f)
{
	if (f->index != 0)
		return -EINVAL;

	f->pixelformat = V4L2_PIX_FMT_YUYV;  // Supported format
	strscpy(f->description, "YUYV 4:2:2", sizeof(f->description));
	f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	return 0;
}

static int custom_g_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
	pr_info("user requested for get fromat \n");
	f->fmt.pix.width       = 640;
	f->fmt.pix.height      = 480;
	f->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	f->fmt.pix.field       = V4L2_FIELD_NONE;
	f->fmt.pix.bytesperline = 640 * 2;
	f->fmt.pix.sizeimage    = 640 * 480 * 2;
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SRGB;
	return 0;
}

static int custom_s_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
	// Accept only YUYV
	pr_info("user requested for set fromat \n");
	if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV)
		return -EINVAL;

	// Clamp resolution (optional)
	f->fmt.pix.width       = 640;
	f->fmt.pix.height      = 480;
	f->fmt.pix.field       = V4L2_FIELD_NONE;
	f->fmt.pix.bytesperline = 640 * 2;
	f->fmt.pix.sizeimage    = 640 * 480 * 2;
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SRGB;
	return 0;
}

static int custom_try_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
	pr_info("user requested for try fromat \n");
	// Similar to s_fmt but does not apply changes
	return custom_s_fmt_vid_cap(file, priv, f);
}

static int custom_querycap(struct file *file, void *priv, struct v4l2_capability *cap)
{    
	pr_info("user requested for device capabilities \n");
	strscpy(cap->driver, "custom_v4l2", sizeof(cap->driver));
	strscpy(cap->card, "Custom V4L2 Capture Device", sizeof(cap->card));
	strscpy(cap->bus_info, "platform:custom", sizeof(cap->bus_info));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	return 0;
}
static int custom_open(struct file *file)
{
	struct video_device *vdev = video_devdata(file);
	int ret;

	ret = v4l2_fh_open(file);
	if (ret)
		return ret;

	file->private_data = video_get_drvdata(vdev);
	return 0;
}

/* Only basic ioctl ops  */
static const struct v4l2_ioctl_ops custom_ioctl_ops = {
	.vidioc_querycap = custom_querycap,
	.vidioc_enum_fmt_vid_cap = custom_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap   = custom_g_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap   = custom_s_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap = custom_try_fmt_vid_cap,
};

/* file operations */
static const struct v4l2_file_operations custom_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = video_ioctl2, // uses ioctl_ops
        .mmap           = vb2_fop_mmap,
	.poll           = vb2_fop_poll,
	.release        = vb2_fop_release,
	.open           = custom_open,
};

static const struct vb2_ops custom_vb2_ops = {
	.queue_setup     = queue_setup,
	.buf_prepare     = buffer_prepare,
	.buf_queue       = buffer_queue,
	.wait_prepare    = vb2_ops_wait_prepare,
	.wait_finish     = vb2_ops_wait_finish,
};



static int __init v4l2_custom_init(void)
{

	pr_info("v4l2 custom initialization \n");
	struct my_stream *stream;
	int ret;
	stream = kzalloc(sizeof(*stream), GFP_KERNEL);
	if (!stream)
		return -ENOMEM;
	/*
	   custom_frame = vzalloc(FRAME_SIZE);
	   if (!custom_frame)
	   return -ENOMEM;

	   memset(custom_frame, 0x80, FRAME_SIZE); // fill with gray
	   */
	//strlcpy(v4l2_dev.name, "v4l2_custom_device", sizeof(v4l2_dev.name));
	strscpy(v4l2_dev.name, "v4l2_custom_device", sizeof(v4l2_dev.name));
	ret = v4l2_device_register(NULL, &v4l2_dev);
	if (ret){
		pr_err("Failed to register v4l2_device\n");
		return ret;
	}

	pr_info("successfully completed v4l2 registration \n");

	vdev = video_device_alloc();
	if (!vdev) {
		kfree(stream);
		pr_err("failed allocated memory for v4l2 device\n ");
		ret = -ENOMEM;
		goto error_device;
	}


	vdev->v4l2_dev = &v4l2_dev;
	vdev->fops = &custom_fops;
	vdev->ioctl_ops = &custom_ioctl_ops;
	vdev->release = video_device_release;
	vdev->vfl_dir = VFL_DIR_RX;
	vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE;
	strscpy(vdev->name,"custom-v4l2", sizeof(vdev->name));

#if 1 
	// Init fake frame
	//frame_data = vzalloc(FRAME_SIZE);
	//if (!frame_data)
	//	return -ENOMEM;
//	memset(frame_data, 0x80, FRAME_SIZE);  // fill with gray

	pr_info("after alloc \n");
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
        if (!dev) {
             pr_err("kzalloc failed \n ");
		return -ENOMEM;
	}

	mutex_init(&dev->lock);
        memset(&dev->vb_queue, 0, sizeof(dev->vb_queue));
	// vb2 queue setup
	vb_queue.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vb_queue.io_modes = VB2_MMAP | VB2_READ;
	vb_queue.drv_priv = vdev;
	vb_queue.buf_struct_size = sizeof(struct vb2_v4l2_buffer);
	vb_queue.ops = &custom_vb2_ops;
	vb_queue.mem_ops = &vb2_vmalloc_memops;
	vb_queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	vb_queue.lock = &dev->lock;

	vb_queue.dev = NULL;

	ret = vb2_queue_init(&vb_queue);
	if (ret) {
		pr_err("vb2_queue_init failed\n");
		goto error_vdev;
	}

	video_set_drvdata(vdev, stream);
	ret = video_register_device(vdev, VFL_TYPE_VIDEO, -1);
	if (ret){
		pr_err("Failed to register video device\n"); 
		goto error_vdev;
	}
	pr_info("v4l2_custom: registered /dev/video%d\n", vdev->num);
	

error_vdev:
	video_device_release(vdev);
#endif
error_device:
	v4l2_device_unregister(&v4l2_dev);
	//error_vzalloc:
	//	vfree(custom_frame);

	return ret;
}



static void __exit v4l2_custom_exit(void)
{
	pr_info("exit from v4l2 \n");
	video_unregister_device(vdev);
	v4l2_device_unregister(&v4l2_dev);
	//vfree(custom_frame);
	pr_info("v4l2_custom: unloaded\n");
}

module_init(v4l2_custom_init);
module_exit(v4l2_custom_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ganga");
MODULE_DESCRIPTION("Simple V4L2 Custom Video Capture Driver");
