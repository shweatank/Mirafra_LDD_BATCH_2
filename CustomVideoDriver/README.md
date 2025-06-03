struct v4l2_device:
Defined in include/media/v4l2-device.h

struct v4l2_device {
    struct device *dev;                     // Associated device (e.g., platform device or USB)
    struct media_device *mdev;             // Media controller integration (optional)
    struct list_head subdevs;              // List of sub-devices (v4l2_subdev)
    spinlock_t lock;                       // Protects internal state
    char name[V4L2_DEVICE_NAME_SIZE];      // Device name
    void (*notify)(struct v4l2_subdev *sd, 
                   unsigned int notification, void *arg); // Notification callback
    struct v4l2_ctrl_handler *ctrl_handler;// Control handler for the whole device
    struct v4l2_subdev *ctrl_subdev;       // Subdev that handles controls
    struct list_head video_dev_list;       // List of associated video_device instances
};



struct video_device:

Defined in include/media/v4l2-dev.h

struct video_device {
    const struct v4l2_file_operations *fops; // File operations (open, release, ioctl, etc.)
    const struct v4l2_ioctl_ops *ioctl_ops;  // V4L2 ioctl callbacks

    struct v4l2_device *v4l2_dev;            // Pointer to the parent v4l2_device

    struct device dev;                       // Embedded device structure
    struct cdev *cdev;                       // Character device structure

    char name[32];                           // Device name
    int minor;                               // Minor number
    int index;                               // Index number among all devices

    enum vfl_devnode_type vfl_type;          // Type of node (VFL_TYPE_VIDEO, etc.)
    enum vfl_devnode_direction vfl_dir;      // Direction: capture or output

    int (*release)(struct video_device *vdev); // Optional release callback

    void *private_data;                      // Driver-specific data
    struct mutex *lock;                      // Used for serialization

    // vb2 support
    struct vb2_queue *queue;                 // vb2 queue pointer

    struct media_entity entity;              // Media controller entity
};

