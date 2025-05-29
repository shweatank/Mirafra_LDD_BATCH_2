#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/videodev2.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <media/v4l2-device.h>
#include <media/v4l2-dev.h>
#include <media/videobuf2-vmalloc.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-v4l2.h>
#define DEVICE_NAME "v4l2_custom"
#define FRAME_SIZE 640 * 480 * 2 // Simulated YUYV frame size

struct custom_dev {
	struct v4l2_device v4l2_dev;
	struct video_device vdev;
	struct vb2_queue queue;
	struct mutex lock;
	struct list_head buffer_list;
	spinlock_t slock;
};

static struct custom_dev *dev;

static int custom_querycap(struct file *file, void *priv, struct v4l2_capability *cap)
{
	strscpy(cap->driver, DEVICE_NAME, sizeof(cap->driver));
	strscpy(cap->card, "Custom Video Device", sizeof(cap->card));
	strscpy(cap->bus_info, "platform:v4l2_custom", sizeof(cap->bus_info));
	cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	return 0;
}

static const struct v4l2_ioctl_ops custom_ioctl_ops = {
	.vidioc_querycap = custom_querycap,
};

static int custom_queue_setup(struct vb2_queue *vq,
			      unsigned int *nbuffers, unsigned int *nplanes,
			      unsigned int sizes[], struct device *alloc_devs[])
{
	*nplanes = 1;
	sizes[0] = FRAME_SIZE;
	return 0;
}

static int custom_buf_prepare(struct vb2_buffer *vb)
{
	return 0;
}

static void custom_buf_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	vbuf->field = V4L2_FIELD_NONE;
	vb2_buffer_done(&vbuf->vb2_buf, VB2_BUF_STATE_DONE);
}

static const struct vb2_ops custom_vb2_ops = {
	.queue_setup     = custom_queue_setup,
	.buf_prepare     = custom_buf_prepare,
	.buf_queue       = custom_buf_queue,
	.wait_prepare    = vb2_ops_wait_prepare,
	.wait_finish     = vb2_ops_wait_finish,
};

static const struct v4l2_file_operations custom_fops = {
	.owner          = THIS_MODULE,
//	.open           = vb2_fop_open,
	.release        = vb2_fop_release,
	.poll           = vb2_fop_poll,
	.mmap           = vb2_fop_mmap,
	.unlocked_ioctl = video_ioctl2,
};

static int __init custom_init(void)
{
	int ret;
        pr_info("custom v4l2 driver initialization \n");
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	mutex_init(&dev->lock);
	INIT_LIST_HEAD(&dev->buffer_list);
	spin_lock_init(&dev->slock);
        strscpy(dev->v4l2_dev.name, "v4l2_custom_device", sizeof(dev->v4l2_dev.name));
	ret = v4l2_device_register(NULL, &dev->v4l2_dev);
	if (ret)
		goto error_free;

	struct vb2_queue *q = &dev->queue;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP;
	q->drv_priv = dev;
	q->buf_struct_size = sizeof(struct vb2_v4l2_buffer);
	q->ops = &custom_vb2_ops;
	q->mem_ops = &vb2_vmalloc_memops;
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	q->lock = &dev->lock;

	ret = vb2_queue_init(q);
	if (ret)
		goto error_unreg;

	struct video_device *vdev = &dev->vdev;
	strscpy(vdev->name, DEVICE_NAME, sizeof(vdev->name));
	vdev->fops = &custom_fops;
	vdev->ioctl_ops = &custom_ioctl_ops;
	vdev->release = video_device_release_empty;
	vdev->v4l2_dev = &dev->v4l2_dev;
	vdev->queue = q;
	vdev->vfl_dir = VFL_DIR_RX;
	vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;

	ret = video_register_device(vdev, VFL_TYPE_VIDEO, -1);
	if (ret)
		goto error_unreg;

	pr_info("v4l2_custom: Registered /dev/video%d\n", vdev->num);
	return 0;

error_unreg:
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
	pr_info("v4l2_custom: Module unloaded\n");
}

module_init(custom_init);
module_exit(custom_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI Example");
MODULE_DESCRIPTION("Minimal V4L2 Capture Driver using VB2");

