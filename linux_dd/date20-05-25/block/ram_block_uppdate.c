#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/blk-mq.h>

#define DEVICE_NAME "ramblk"
#define NSECTORS 2048
#define SECTOR_SIZE 512
#define RAMDISK_SIZE (NSECTORS * SECTOR_SIZE)

static struct ramblk_dev {
    struct blk_mq_tag_set tag_set;
    struct request_queue *queue;
    struct gendisk *disk;
    u8 *data;
    spinlock_t lock;
} *ramdev;

static int major = 0;

static void ramblk_transfer(struct ramblk_dev *dev, sector_t sector,
                            unsigned long nsect, char *buffer, int write)
{
    unsigned long offset = sector * SECTOR_SIZE;
    unsigned long nbytes = nsect * SECTOR_SIZE;

    if ((offset + nbytes) > RAMDISK_SIZE) {
        pr_warn("ramblk: write/read out of bounds!\n");
        return;
    }

    if (write)
        memcpy(dev->data + offset, buffer, nbytes);
    else
        memcpy(buffer, dev->data + offset, nbytes);
}

static blk_status_t ramblk_queue_rq(struct blk_mq_hw_ctx *hctx,
                                    const struct blk_mq_queue_data *bd)
{
    struct request *req = bd->rq;
    struct ramblk_dev *dev = ramdev;
    struct bio_vec bvec;
    struct req_iterator iter;
    sector_t sector = blk_rq_pos(req);
    unsigned int sectors;

    if (blk_rq_is_passthrough(req)) {
        pr_err("ramblk: Skip non-fs request\n");
        blk_mq_end_request(req, BLK_STS_IOERR);
        return BLK_STS_IOERR;
    }

    blk_mq_start_request(req);

    rq_for_each_segment(bvec, req, iter) {
        char *buffer = kmap_local_page(bvec.bv_page) + bvec.bv_offset;
        sectors = bvec.bv_len / SECTOR_SIZE;
        ramblk_transfer(dev, sector, sectors, buffer, rq_data_dir(req));
        kunmap_local(buffer);
        sector += sectors;
    }

    blk_mq_end_request(req, BLK_STS_OK);
    return BLK_STS_OK;
}

static const struct blk_mq_ops ramblk_mq_ops = {
    .queue_rq = ramblk_queue_rq,
};

static int __init ramblk_init(void)
{
    ramdev = kzalloc(sizeof(struct ramblk_dev), GFP_KERNEL);
    if (!ramdev)
        return -ENOMEM;

    ramdev->data = vmalloc(RAMDISK_SIZE);
    if (!ramdev->data) {
        kfree(ramdev);
        return -ENOMEM;
    }

    spin_lock_init(&ramdev->lock);

    // Setup tag set
    ramdev->tag_set.ops = &ramblk_mq_ops;
    ramdev->tag_set.nr_hw_queues = 1;
    ramdev->tag_set.queue_depth = 128;
    ramdev->tag_set.numa_node = NUMA_NO_NODE;
    ramdev->tag_set.cmd_size = 0;
    ramdev->tag_set.flags = BLK_MQ_F_SHOULD_MERGE;
    ramdev->tag_set.driver_data = ramdev;

    if (blk_mq_alloc_tag_set(&ramdev->tag_set)) {
        vfree(ramdev->data);
        kfree(ramdev);
        return -ENOMEM;
    }

    ramdev->disk = blk_mq_alloc_disk(&ramdev->tag_set, ramdev);
    if (IS_ERR(ramdev->disk)) {
        blk_mq_free_tag_set(&ramdev->tag_set);
        vfree(ramdev->data);
        kfree(ramdev);
        return PTR_ERR(ramdev->disk);
    }

    ramdev->disk->major = 0;  // auto-assign
    ramdev->disk->first_minor = 0;
    ramdev->disk->minors = 1;
    ramdev->disk->fops = NULL; // Can be extended
    ramdev->disk->private_data = ramdev;
    snprintf(ramdev->disk->disk_name, 16, DEVICE_NAME);
    set_capacity(ramdev->disk, NSECTORS);

    add_disk(ramdev->disk);

    major = ramdev->disk->major;

    pr_info("ramblk: loaded (size = %lu KB, major = %d)\n", RAMDISK_SIZE / 1024, major);
    return 0;
}

static void __exit ramblk_exit(void)
{
    del_gendisk(ramdev->disk);
    blk_cleanup_disk(ramdev->disk);
    blk_mq_free_tag_set(&ramdev->tag_set);
    vfree(ramdev->data);
    kfree(ramdev);
    pr_info("ramblk: unloaded\n");
}

module_init(ramblk_init);
module_exit(ramblk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant @ TechDhaba");
MODULE_DESCRIPTION("RAM Block Driver (Kernel 6.1+)");
