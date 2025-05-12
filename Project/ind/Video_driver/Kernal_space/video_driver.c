// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <linux/fs.h>

#define DEVICE_NAME "vvd_camera"

static struct video_device *vvd_vdev;
static struct v4l2_device vvd_v4l2_dev;

static int vvd_open(struct file *file)
{
    pr_info("vvd: device opened\n");
    return 0;
}

static int vvd_release(struct file *file)
{
    pr_info("vvd: device closed\n");
    return 0;
}

static const struct v4l2_file_operations vvd_fops = {
    .owner = THIS_MODULE,
    .open = vvd_open,
    .release = vvd_release,
};

static int __init vvd_init(void)
{
    int ret;

    pr_info("vvd: Initializing virtual video driver...\n");

    // Register V4L2 device with the system
    ret = v4l2_device_register(NULL, &vvd_v4l2_dev);
    if (ret) {
        pr_err("vvd: Failed to register v4l2_device\n");
        return ret;
    }

    // Allocate video device
    vvd_vdev = video_device_alloc();
    if (!vvd_vdev) {
        v4l2_device_unregister(&vvd_v4l2_dev);
        return -ENOMEM;
    }

    strscpy(vvd_vdev->name, DEVICE_NAME, sizeof(vvd_vdev->name));
    vvd_vdev->fops = &vvd_fops;
    vvd_vdev->release = video_device_release;
    vvd_vdev->v4l2_dev = &vvd_v4l2_dev;

    // Register as video device (use VFL_TYPE_VIDEO instead of deprecated GRABBER)
    ret = video_register_device(vvd_vdev, VFL_TYPE_VIDEO, -1);
    if (ret < 0) {
        video_device_release(vvd_vdev);
        v4l2_device_unregister(&vvd_v4l2_dev);
        return ret;
    }

    pr_info("vvd: Registered virtual video device as /dev/video%d\n", vvd_vdev->num);
    return 0;
}

static void __exit vvd_exit(void)
{
    pr_info("vvd: Cleaning up virtual video driver\n");
    video_unregister_device(vvd_vdev);
    v4l2_device_unregister(&vvd_v4l2_dev);
}

module_init(vvd_init);
module_exit(vvd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash");
MODULE_DESCRIPTION("Simple Virtual V4L2 Video Driver for LDD Project");

