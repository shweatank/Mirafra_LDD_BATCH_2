/* virtual_cam.c - A custom UVC camera kernel driver registered as /dev/video20 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/videodev2.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-dev.h>

#define DRIVER_NAME "uvc-custom-cam"
#define VIDEO_DEVICE_NUM 20 /* Register as /dev/video20 */
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define MAX_BUFFERS 4

/* USB Vendor and Product ID for the HP HD Camera */
#define CAMERA_VENDOR_ID 0x04ca
#define CAMERA_PRODUCT_ID 0x7054

/* UVC-specific defines */
#define UVC_GET_CUR 0x81
#define UVC_SET_CUR 0x01
#define UVC_VS_PROBE_CONTROL 0x01
#define UVC_VS_COMMIT_CONTROL 0x02

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Custom Driver");
MODULE_DESCRIPTION("Custom UVC Camera Driver for /dev/video20");

struct uvc_custom_buffer {
    struct v4l2_buffer buf;
    bool in_queue;
    void *data;
    unsigned long offset;
};

struct uvc_custom_device {
    struct v4l2_device v4l2_dev;
    struct video_device *vdev;
    struct usb_device *udev;
    struct usb_interface *interface;
    struct v4l2_format fmt;
    struct mutex lock;
    u8 *frame_data; /* Buffer for frame data */
    struct uvc_custom_buffer buffers[MAX_BUFFERS];
    int num_buffers;
    bool streaming;
    struct urb *urb; /* USB Request Block for streaming */
    u8 *urb_buffer;
    dma_addr_t urb_dma; /* DMA address for URB buffer */
    int streaming_interface;
    int streaming_endpoint;
    bool frame_ready; /* Flag to indicate a frame is ready */
    bool is_isochronous; /* Flag to indicate if endpoint is isochronous */
};

/* Global pointer to the device */
static struct uvc_custom_device *cam_dev;
static struct usb_driver uvc_custom_driver;

/* UVC Probe Control Structure (simplified) */
struct uvc_probe_control {
    __u16 bmHint;
    __u8 bFormatIndex;
    __u8 bFrameIndex;
    __u32 dwFrameInterval;
    __u16 wKeyFrameRate;
    __u16 wPFrameRate;
    __u16 wCompQuality;
    __u16 wCompWindowSize;
    __u16 wDelay;
    __u32 dwMaxVideoFrameSize;
    __u32 dwMaxPayloadTransferSize;
    __u32 dwClockFrequency;
    __u8 bmFramingInfo;
    __u8 bPreferedVersion;
    __u8 bMinVersion;
    __u8 bMaxVersion;
} __attribute__((packed));

/* URB completion handler */
static void uvc_custom_urb_complete(struct urb *urb)
{
    struct uvc_custom_device *dev = urb->context;
    int ret;

    if (urb->status) {
        printk(KERN_ERR "UVC Custom Cam: URB error %d\n", urb->status);
        return;
    }

    /* Copy the received data to the frame buffer (simplified) */
    if (urb->actual_length > 0 && urb->actual_length <= FRAME_WIDTH * FRAME_HEIGHT * 2) {
        memcpy(dev->frame_data, dev->urb_buffer, urb->actual_length);
        dev->frame_ready = true; /* Mark frame as ready */
        printk(KERN_DEBUG "UVC Custom Cam: Received %d bytes\n", urb->actual_length);
    } else {
        printk(KERN_WARNING "UVC Custom Cam: Invalid URB length %d\n", urb->actual_length);
    }

    /* Resubmit the URB to keep streaming */
    if (dev->streaming) {
        ret = usb_submit_urb(urb, GFP_ATOMIC);
        if (ret) {
            printk(KERN_ERR "UVC Custom Cam: Failed to resubmit URB: %d\n", ret);
        }
    }
}

/* Configure the camera and start streaming */
static int uvc_custom_start_streaming(struct uvc_custom_device *dev)
{
    struct usb_interface *intf = dev->interface;
    struct usb_host_interface *alts;
    struct uvc_probe_control probe;
    int ret, i, alt_setting;
    int retries = 3;
    int num_alt_settings = intf->num_altsetting;

    /* Ensure the interface is in a known state */
    ret = usb_set_interface(dev->udev, dev->streaming_interface, 0);
    if (ret) {
        printk(KERN_ERR "UVC Custom Cam: Failed to reset streaming interface: %d\n", ret);
        return ret;
    }

    /* Try each alternate setting until probe control succeeds */
    for (alt_setting = 1; alt_setting < num_alt_settings; alt_setting++) {
        printk(KERN_DEBUG "UVC Custom Cam: Trying alternate setting %d for interface %d\n",
               alt_setting, dev->streaming_interface);
        ret = usb_set_interface(dev->udev, dev->streaming_interface, alt_setting);
        if (ret) {
            printk(KERN_ERR "UVC Custom Cam: Failed to set interface %d to alternate setting %d: %d\n",
                   dev->streaming_interface, alt_setting, ret);
            continue; /* Try the next alternate setting */
        }

        /* Increase delay to ensure the camera is ready */
        msleep(500);

        /* Configure the camera with a probe control (set format to YUYV, 640x480) */
        memset(&probe, 0, sizeof(probe));
        probe.bmHint = 1; /* Fix format and frame index */
        probe.bFormatIndex = 1; /* Start with index 1, adjust later with v4l2-ctl */
        probe.bFrameIndex = 1; /* Start with index 1, adjust later with v4l2-ctl */
        probe.dwFrameInterval = 333333; /* 30 fps (100ns units) */
        probe.dwMaxVideoFrameSize = FRAME_WIDTH * FRAME_HEIGHT * 2;
        probe.dwMaxPayloadTransferSize = 1024; /* Increased for better compatibility */
        probe.dwClockFrequency = 48000000; /* Common UVC clock frequency */
        probe.bmFramingInfo = 0;
        probe.bPreferedVersion = 0;
        probe.bMinVersion = 0;
        probe.bMaxVersion = 0;

        printk(KERN_DEBUG "UVC Custom Cam: Sending probe control: bFormatIndex=%d, bFrameIndex=%d, dwFrameInterval=%d\n",
               probe.bFormatIndex, probe.bFrameIndex, probe.dwFrameInterval);

        /* Retry the probe control if it fails with EAGAIN */
        retries = 3;
        while (retries--) {
            ret = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
                                  UVC_SET_CUR, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
                                  UVC_VS_PROBE_CONTROL << 8, dev->streaming_interface,
                                  &probe, sizeof(probe), 5000);
            if (ret >= 0) {
                break;
            }
            if (ret != -EAGAIN) {
                printk(KERN_ERR "UVC Custom Cam: Failed to set probe control: %d\n", ret);
                break; /* Non-recoverable error, try next alternate setting */
            }
            printk(KERN_WARNING "UVC Custom Cam: Probe control failed with EAGAIN, retrying (%d retries left)\n", retries);
            msleep(200); /* Increased delay before retrying */
        }
        if (ret >= 0) {
            break; /* Success, proceed with this alternate setting */
        }
        printk(KERN_WARNING "UVC Custom Cam: Failed to set probe control with alternate setting %d after retries: %d\n",
               alt_setting, ret);
    }

    if (ret < 0) {
        printk(KERN_ERR "UVC Custom Cam: Failed to set probe control with any alternate setting\n");
        return ret;
    }

    /* Commit the settings */
    ret = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
                          UVC_SET_CUR, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
                          UVC_VS_COMMIT_CONTROL << 8, dev->streaming_interface,
                          &probe, sizeof(probe), 5000);
    if (ret < 0) {
        printk(KERN_ERR "UVC Custom Cam: Failed to commit control: %d\n", ret);
        return ret;
    }

    /* Find the streaming endpoint */
    alts = &intf->altsetting[alt_setting]; /* Use the selected alternate setting */
    dev->streaming_endpoint = 0;
    dev->is_isochronous = false;
    for (i = 0; i < alts->desc.bNumEndpoints; i++) {
        if ((alts->endpoint[i].desc.bEndpointAddress & USB_DIR_IN) &&
            (alts->endpoint[i].desc.bmAttributes & USB_ENDPOINT_XFER_ISOC)) {
            dev->streaming_endpoint = alts->endpoint[i].desc.bEndpointAddress;
            dev->is_isochronous = true;
            break;
        }
    }
    if (!dev->streaming_endpoint) {
        printk(KERN_WARNING "UVC Custom Cam: No isochronous endpoint found, trying bulk\n");
        for (i = 0; i < alts->desc.bNumEndpoints; i++) {
            if ((alts->endpoint[i].desc.bEndpointAddress & USB_DIR_IN) &&
                (alts->endpoint[i].desc.bmAttributes & USB_ENDPOINT_XFER_BULK)) {
                dev->streaming_endpoint = alts->endpoint[i].desc.bEndpointAddress;
                dev->is_isochronous = false;
                break;
            }
        }
        if (!dev->streaming_endpoint) {
            printk(KERN_ERR "UVC Custom Cam: No streaming endpoint found\n");
            return -EINVAL;
        }
    }

    /* Allocate URB and buffer for streaming */
    dev->urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!dev->urb) {
        printk(KERN_ERR "UVC Custom Cam: Failed to allocate URB\n");
        return -ENOMEM;
    }

    dev->urb_buffer = usb_alloc_coherent(dev->udev, 1024, GFP_KERNEL, &dev->urb_dma);
    if (!dev->urb_buffer) {
        printk(KERN_ERR "UVC Custom Cam: Failed to allocate URB buffer\n");
        usb_free_urb(dev->urb);
        dev->urb = NULL;
        return -ENOMEM;
    }

    /* Setup the URB for transfer */
    if (dev->is_isochronous) {
        usb_fill_int_urb(dev->urb, dev->udev,
                         usb_rcvisocpipe(dev->udev, dev->streaming_endpoint),
                         dev->urb_buffer, 1024, uvc_custom_urb_complete,
                         dev, 1);
        dev->urb->transfer_dma = dev->urb_dma;
        dev->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    } else {
        usb_fill_bulk_urb(dev->urb, dev->udev,
                          usb_rcvbulkpipe(dev->udev, dev->streaming_endpoint),
                          dev->urb_buffer, 1024, uvc_custom_urb_complete,
                          dev);
        dev->urb->transfer_dma = dev->urb_dma;
        dev->urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
    }

    /* Submit the URB to start streaming */
    ret = usb_submit_urb(dev->urb, GFP_KERNEL);
    if (ret) {
        printk(KERN_ERR "UVC Custom Cam: Failed to submit URB: %d\n", ret);
        usb_free_coherent(dev->udev, 1024, dev->urb_buffer, dev->urb_dma);
        usb_free_urb(dev->urb);
        dev->urb = NULL;
        dev->urb_buffer = NULL;
        return ret;
    }

    printk(KERN_INFO "UVC Custom Cam: Started streaming with alternate setting %d\n", alt_setting);
    return 0;
}

/* Stop streaming */
static void uvc_custom_stop_streaming(struct uvc_custom_device *dev)
{
    if (dev->urb) {
        usb_kill_urb(dev->urb);
        usb_free_coherent(dev->udev, 1024, dev->urb_buffer, dev->urb_dma);
        usb_free_urb(dev->urb);
        dev->urb = NULL;
        dev->urb_buffer = NULL;
    }
    usb_set_interface(dev->udev, dev->streaming_interface, 0);
    dev->is_isochronous = false;
    printk(KERN_INFO "UVC Custom Cam: Stopped streaming\n");
}

/* Capture a frame (wait for URB to fill the buffer) */
static int capture_frame(struct uvc_custom_device *dev)
{
    int timeout = 100; /* Wait up to 1 second (100 * 10ms) */

    if (!dev->streaming) {
        printk(KERN_ERR "UVC Custom Cam: Streaming not started\n");
        return -EINVAL;
    }

    /* Wait for a frame to be ready */
    while (!dev->frame_ready && timeout--) {
        msleep(10);
    }

    if (!dev->frame_ready) {
        printk(KERN_ERR "UVC Custom Cam: Timed out waiting for frame\n");
        return -ETIMEDOUT;
    }

    dev->frame_ready = false; /* Reset the flag */
    return 0;
}

static int uvc_custom_open(struct file *file)
{
    printk(KERN_INFO "UVC Custom Cam: Device opened\n");
    file->private_data = cam_dev;
    return 0;
}

static int uvc_custom_release(struct file *file)
{
    printk(KERN_INFO "UVC Custom Cam: Device released\n");
    return 0;
}

static ssize_t uvc_custom_read(struct file *file, char __user *buf,
                               size_t count, loff_t *ppos)
{
    size_t frame_size = FRAME_WIDTH * FRAME_HEIGHT * 2;

    if (*ppos >= frame_size) {
        return 0;
    }

    if (count > frame_size - *ppos)
        count = frame_size - *ppos;

    if (copy_to_user(buf, cam_dev->frame_data + *ppos, count)) {
        printk(KERN_ERR "UVC Custom Cam: Failed to copy data to user\n");
        return -EFAULT;
    }

    *ppos += count;
    return count;
}

static int uvc_custom_querycap(struct file *file, void *priv,
                               struct v4l2_capability *cap)
{
    strscpy(cap->driver, DRIVER_NAME, sizeof(cap->driver));
    strscpy(cap->card, "Custom UVC Camera", sizeof(cap->card));
    strscpy(cap->bus_info, "usb", sizeof(cap->bus_info));
    cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;
    cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
    return 0;
}

static int uvc_custom_enum_fmt(struct file *file, void *priv,
                               struct v4l2_fmtdesc *f)
{
    if (f->index > 0) {
        return -EINVAL;
    }
    f->pixelformat = V4L2_PIX_FMT_YUYV;
    strscpy(f->description, "YUYV 4:2:2", sizeof(f->description));
    f->flags = 0;
    return 0;
}

static int uvc_custom_g_fmt(struct file *file, void *priv,
                            struct v4l2_format *f)
{
    memcpy(f, &cam_dev->fmt, sizeof(*f));
    return 0;
}

static int uvc_custom_s_fmt(struct file *file, void *priv,
                            struct v4l2_format *f)
{
    if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV ||
        f->fmt.pix.width != FRAME_WIDTH ||
        f->fmt.pix.height != FRAME_HEIGHT) {
        return -EINVAL;
    }
    memcpy(&cam_dev->fmt, f, sizeof(*f));
    return 0;
}

static int uvc_custom_g_parm(struct file *file, void *priv,
                             struct v4l2_streamparm *parm)
{
    if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        return -EINVAL;
    }

    parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    parm->parm.capture.capturemode = 0;
    parm->parm.capture.timeperframe.numerator = 333333; /* 30 fps in 100ns units */
    parm->parm.capture.timeperframe.denominator = 10000000;
    parm->parm.capture.extendedmode = 0;
    parm->parm.capture.readbuffers = 2;

    return 0;
}

static int uvc_custom_g_input(struct file *file, void *priv, unsigned int *i)
{
    *i = 0;
    return 0;
}

static int uvc_custom_s_input(struct file *file, void *priv, unsigned int i)
{
    if (i != 0) {
        return -EINVAL;
    }
    return 0;
}

static int uvc_custom_enum_input(struct file *file, void *priv,
                                 struct v4l2_input *inp)
{
    if (inp->index != 0) {
        return -EINVAL;
    }
    strscpy(inp->name, "Camera Input", sizeof(inp->name));
    inp->type = V4L2_INPUT_TYPE_CAMERA;
    return 0;
}

static int uvc_custom_reqbufs(struct file *file, void *priv,
                              struct v4l2_requestbuffers *rb)
{
    int i;

    if (rb->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || rb->memory != V4L2_MEMORY_MMAP) {
        return -EINVAL;
    }

    for (i = 0; i < cam_dev->num_buffers; i++) {
        if (cam_dev->buffers[i].data) {
            vfree(cam_dev->buffers[i].data);
            cam_dev->buffers[i].data = NULL;
        }
    }
    cam_dev->num_buffers = 0;

    if (rb->count > MAX_BUFFERS)
        rb->count = MAX_BUFFERS;
    if (rb->count < 1) {
        rb->count = 0;
        return 0;
    }

    for (i = 0; i < rb->count; i++) {
        cam_dev->buffers[i].data = vmalloc_user(FRAME_WIDTH * FRAME_HEIGHT * 2);
        if (!cam_dev->buffers[i].data) {
            for (; i >= 0; i--) {
                vfree(cam_dev->buffers[i].data);
                cam_dev->buffers[i].data = NULL;
            }
            return -ENOMEM;
        }
        cam_dev->buffers[i].buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        cam_dev->buffers[i].buf.memory = V4L2_MEMORY_MMAP;
        cam_dev->buffers[i].buf.index = i;
        cam_dev->buffers[i].buf.length = FRAME_WIDTH * FRAME_HEIGHT * 2;
        cam_dev->buffers[i].in_queue = false;
        cam_dev->buffers[i].offset = i * (FRAME_WIDTH * FRAME_HEIGHT * 2);
    }

    cam_dev->num_buffers = rb->count;
    return 0;
}

static int uvc_custom_querybuf(struct file *file, void *priv,
                               struct v4l2_buffer *buf)
{
    if (buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || buf->memory != V4L2_MEMORY_MMAP ||
        buf->index >= cam_dev->num_buffers) {
        return -EINVAL;
    }

    buf->length = FRAME_WIDTH * FRAME_HEIGHT * 2;
    buf->m.offset = cam_dev->buffers[buf->index].offset;
    buf->flags = V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
    return 0;
}

static int uvc_custom_qbuf(struct file *file, void *priv,
                           struct v4l2_buffer *buf)
{
    if (buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || buf->memory != V4L2_MEMORY_MMAP ||
        buf->index >= cam_dev->num_buffers) {
        return -EINVAL;
    }

    if (cam_dev->buffers[buf->index].in_queue) {
        return -EBUSY;
    }

    cam_dev->buffers[buf->index].in_queue = true;
    buf->flags |= V4L2_BUF_FLAG_QUEUED;
    buf->flags &= ~V4L2_BUF_FLAG_DONE;
    return 0;
}

static int uvc_custom_dqbuf(struct file *file, void *priv,
                            struct v4l2_buffer *buf)
{
    int i;

    if (buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || buf->memory != V4L2_MEMORY_MMAP ||
        buf->index >= cam_dev->num_buffers) {
        return -EINVAL;
    }

    for (i = 0; i < cam_dev->num_buffers; i++) {
        if (cam_dev->buffers[i].in_queue && cam_dev->streaming) {
            /* Capture a frame from the camera */
            if (capture_frame(cam_dev)) {
                return -EIO;
            }
            memcpy(cam_dev->buffers[i].data, cam_dev->frame_data, FRAME_WIDTH * FRAME_HEIGHT * 2);

            cam_dev->buffers[i].in_queue = false;
            buf->index = i;
            buf->bytesused = FRAME_WIDTH * FRAME_HEIGHT * 2;
            buf->flags = V4L2_BUF_FLAG_DONE | V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
            buf->sequence = 0;
            ktime_t ts = ktime_get();
            buf->timestamp.tv_sec = ts / 1000000000;
            buf->timestamp.tv_usec = (ts % 1000000000) / 1000;
            return 0;
        }
    }

    return -EINVAL;
}

static int uvc_custom_streamon(struct file *file, void *priv, enum v4l2_buf_type type)
{
    if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        return -EINVAL;
    }
    cam_dev->streaming = true;
    cam_dev->frame_ready = false;
    return uvc_custom_start_streaming(cam_dev);
}

static int uvc_custom_streamoff(struct file *file, void *priv, enum v4l2_buf_type type)
{
    int i;

    if (type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        return -EINVAL;
    }
    cam_dev->streaming = false;
    cam_dev->frame_ready = false;
    for (i = 0; i < cam_dev->num_buffers; i++)
        cam_dev->buffers[i].in_queue = false;
    uvc_custom_stop_streaming(cam_dev);
    return 0;
}

static int uvc_custom_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long size = vma->vm_end - vma->vm_start;
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
    unsigned int index;

    for (index = 0; index < cam_dev->num_buffers; index++) {
        if (cam_dev->buffers[index].offset == offset)
            break;
    }

    if (index >= cam_dev->num_buffers || size > FRAME_WIDTH * FRAME_HEIGHT * 2) {
        return -EINVAL;
    }

    if (remap_vmalloc_range(vma, cam_dev->buffers[index].data, 0)) {
        return -ENOMEM;
    }

    return 0;
}

static const struct v4l2_ioctl_ops uvc_custom_ioctl_ops = {
    .vidioc_querycap = uvc_custom_querycap,
    .vidioc_enum_fmt_vid_cap = uvc_custom_enum_fmt,
    .vidioc_g_fmt_vid_cap = uvc_custom_g_fmt,
    .vidioc_s_fmt_vid_cap = uvc_custom_s_fmt,
    .vidioc_g_parm = uvc_custom_g_parm,
    .vidioc_g_input = uvc_custom_g_input,
    .vidioc_s_input = uvc_custom_s_input,
    .vidioc_enum_input = uvc_custom_enum_input,
    .vidioc_reqbufs = uvc_custom_reqbufs,
    .vidioc_querybuf = uvc_custom_querybuf,
    .vidioc_qbuf = uvc_custom_qbuf,
    .vidioc_dqbuf = uvc_custom_dqbuf,
    .vidioc_streamon = uvc_custom_streamon,
    .vidioc_streamoff = uvc_custom_streamoff,
};

static const struct v4l2_file_operations uvc_custom_fops = {
    .owner = THIS_MODULE,
    .open = uvc_custom_open,
    .release = uvc_custom_release,
    .read = uvc_custom_read,
    .unlocked_ioctl = video_ioctl2,
    .mmap = uvc_custom_mmap,
};

static int uvc_custom_probe(struct usb_interface *interface,
                            const struct usb_device_id *id)
{
    int ret;
    struct usb_host_interface *host_intf = interface->cur_altsetting;

    /* Only bind to the streaming interface (interface 1) */
    if (host_intf->desc.bInterfaceNumber != 1) {
        printk(KERN_INFO "UVC Custom Cam: Skipping interface %d (not streaming interface)\n",
               host_intf->desc.bInterfaceNumber);
        return -ENODEV;
    }

    printk(KERN_INFO "UVC Custom Cam: Probing USB device %04x:%04x on interface %d\n",
           id->idVendor, id->idProduct, host_intf->desc.bInterfaceNumber);

    cam_dev = kzalloc(sizeof(*cam_dev), GFP_KERNEL);
    if (!cam_dev) {
        printk(KERN_ERR "UVC Custom Cam: Failed to allocate device\n");
        return -ENOMEM;
    }

    cam_dev->udev = usb_get_dev(interface_to_usbdev(interface));
    cam_dev->interface = interface;

    strscpy(cam_dev->v4l2_dev.name, DRIVER_NAME, sizeof(cam_dev->v4l2_dev.name));
    ret = v4l2_device_register(&cam_dev->udev->dev, &cam_dev->v4l2_dev);
    if (ret) {
        printk(KERN_ERR "UVC Custom Cam: Failed to register V4L2 device\n");
        kfree(cam_dev);
        return ret;
    }

    mutex_init(&cam_dev->lock);

    cam_dev->frame_data = vmalloc_user(FRAME_WIDTH * FRAME_HEIGHT * 2);
    if (!cam_dev->frame_data) {
        printk(KERN_ERR "UVC Custom Cam: Failed to allocate frame buffer\n");
        v4l2_device_unregister(&cam_dev->v4l2_dev);
        kfree(cam_dev);
        return -ENOMEM;
    }

    cam_dev->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cam_dev->fmt.fmt.pix.width = FRAME_WIDTH;
    cam_dev->fmt.fmt.pix.height = FRAME_HEIGHT;
    cam_dev->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    cam_dev->fmt.fmt.pix.field = V4L2_FIELD_NONE;
    cam_dev->fmt.fmt.pix.bytesperline = FRAME_WIDTH * 2;
    cam_dev->fmt.fmt.pix.sizeimage = FRAME_WIDTH * FRAME_HEIGHT * 2;

    cam_dev->vdev = video_device_alloc();
    if (!cam_dev->vdev) {
        printk(KERN_ERR "UVC Custom Cam: Failed to allocate video device\n");
        vfree(cam_dev->frame_data);
        v4l2_device_unregister(&cam_dev->v4l2_dev);
        kfree(cam_dev);
        return -ENOMEM;
    }

    strscpy(cam_dev->vdev->name, DRIVER_NAME, sizeof(cam_dev->vdev->name));
    cam_dev->vdev->fops = &uvc_custom_fops;
    cam_dev->vdev->ioctl_ops = &uvc_custom_ioctl_ops;
    cam_dev->vdev->release = video_device_release;
    cam_dev->vdev->v4l2_dev = &cam_dev->v4l2_dev;
    cam_dev->vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE | V4L2_CAP_STREAMING;
    cam_dev->vdev->vfl_dir = VFL_DIR_RX;

    ret = video_register_device(cam_dev->vdev, VFL_TYPE_VIDEO, VIDEO_DEVICE_NUM);
    if (ret) {
        printk(KERN_ERR "UVC Custom Cam: Failed to register video device\n");
        video_device_release(cam_dev->vdev);
        vfree(cam_dev->frame_data);
        v4l2_device_unregister(&cam_dev->v4l2_dev);
        kfree(cam_dev);
        return ret;
    }

    usb_set_intfdata(interface, cam_dev);
    printk(KERN_INFO "UVC Custom Cam: Registered as /dev/video%d\n", cam_dev->vdev->num);
    return 0;
}

static void uvc_custom_disconnect(struct usb_interface *interface)
{
    struct uvc_custom_device *dev = usb_get_intfdata(interface);
    int i;

    printk(KERN_INFO "UVC Custom Cam: Disconnecting\n");
    uvc_custom_stop_streaming(dev);
    video_unregister_device(dev->vdev);
    vfree(dev->frame_data);
    for (i = 0; i < dev->num_buffers; i++)
        vfree(dev->buffers[i].data);
    v4l2_device_unregister(&cam_dev->v4l2_dev);
    usb_put_dev(dev->udev);
    kfree(dev);
    cam_dev = NULL; /* Reset global pointer */
}

static const struct usb_device_id uvc_custom_id_table[] = {
    { USB_DEVICE(CAMERA_VENDOR_ID, CAMERA_PRODUCT_ID) },
    { } /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, uvc_custom_id_table);

static struct usb_driver uvc_custom_driver = {
    .name = DRIVER_NAME,
    .probe = uvc_custom_probe,
    .disconnect = uvc_custom_disconnect,
    .id_table = uvc_custom_id_table,
};

static int __init uvc_custom_init(void)
{
    int ret;

    ret = usb_register(&uvc_custom_driver);
    if (ret) {
        printk(KERN_ERR "UVC Custom Cam: Failed to register USB driver\n");
        return ret;
    }

    printk(KERN_INFO "UVC Custom Cam: Driver loaded\n");
    return 0;
}

static void __exit uvc_custom_exit(void)
{
    usb_deregister(&uvc_custom_driver);
    printk(KERN_INFO "UVC Custom Cam: Driver unloaded\n");
}

module_init(uvc_custom_init);
module_exit(uvc_custom_exit);
