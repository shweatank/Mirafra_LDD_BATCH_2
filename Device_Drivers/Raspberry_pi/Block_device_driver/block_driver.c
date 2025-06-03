#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/vmalloc.h>
#include <linux/hdreg.h>

#define DEVICE_NAME "ram_block"
#define RAMBLOCK_SECTOR_SIZE 512
#define RAMBLOCK_NUM_SECTORS 1024  // 512 KB

static int major_num = 0;
static struct request_queue *ramblock_queue;
static struct gendisk *ramblock_disk;
static u8 *ramblock_data;

static void ramblock_transfer(struct request *req)
{
    struct bio_vec bvec;
    struct req_iterator iter;
    sector_t sector_offset = 0;
    unsigned int sectors;
    void *buffer;

    rq_for_each_segment(bvec, req, iter) {
        buffer = page_address(bvec.bv_page) + bvec.bv_offset;
        sectors = bvec.bv_len / RAMBLOCK_SECTOR_SIZE;

        if (rq_data_dir(req) == WRITE) {
            memcpy(ramblock_data + (iter.iter.bi_sector + sector_offset) * RAMBLOCK_SECTOR_SIZE, buffer, sectors * RAMBLOCK_SECTOR_SIZE);
        } else {
            memcpy(buffer, ramblock_data + (iter.iter.bi_sector + sector_offset) * RAMBLOCK_SECTOR_SIZE, sectors * RAMBLOCK_SECTOR_SIZE);
        }
        sector_offset += sectors;
    }
}

static blk_status_t ramblock_request(struct blk_mq_hw_ctx *hctx, const struct blk_mq_queue_data *bd)
{
    struct request *req = bd->rq;

    blk_mq_start_request(req);
    ramblock_transfer(req);
    blk_mq_end_request(req, BLK_STS_OK);
    return BLK_STS_OK;
}

static struct blk_mq_ops ramblock_ops = {
    .queue_rq = ramblock_request,
};

static struct blk_mq_tag_set tag_set;

static int __init ramblock_init(void)
{
    ramblock_data = vmalloc(RAMBLOCK_NUM_SECTORS * RAMBLOCK_SECTOR_SIZE);
    if (!ramblock_data) {
        printk(KERN_ERR "ram_block: vmalloc failure.\n");
        return -ENOMEM;
    }

    major_num = register_blkdev(0, DEVICE_NAME);
    if (major_num <= 0) {
        printk(KERN_ERR "ram_block: Unable to get major number.\n");
        vfree(ramblock_data);
        return -EBUSY;
    }

    memset(&tag_set, 0, sizeof(tag_set));
    tag_set.ops = &ramblock_ops;
    tag_set.nr_hw_queues = 1;
    tag_set.queue_depth = 128;
    tag_set.numa_node = NUMA_NO_NODE;
    tag_set.cmd_size = 0;
    tag_set.flags = BLK_MQ_F_SHOULD_MERGE;
    tag_set.driver_data = NULL;

    if (blk_mq_alloc_tag_set(&tag_set)) {
        printk(KERN_ERR "ram_block: Failed to allocate tag set.\n");
        unregister_blkdev(major_num, DEVICE_NAME);
        vfree(ramblock_data);
        return -ENOMEM;
    }

    ramblock_queue = blk_mq_init_queue(&tag_set);
    if (IS_ERR(ramblock_queue)) {
        printk(KERN_ERR "ram_block: Failed to initialize queue.\n");
        blk_mq_free_tag_set(&tag_set);
        unregister_blkdev(major_num, DEVICE_NAME);
        vfree(ramblock_data);
        return PTR_ERR(ramblock_queue);
    }

    ramblock_disk = alloc_disk(1);
    if (!ramblock_disk) {
        printk(KERN_ERR "ram_block: alloc_disk failure.\n");
        blk_cleanup_queue(ramblock_queue);
        blk_mq_free_tag_set(&tag_set);
        unregister_blkdev(major_num, DEVICE_NAME);
        vfree(ramblock_data);
        return -ENOMEM;
    }

    ramblock_disk->major = major_num;
    ramblock_disk->first_minor = 0;
    ramblock_disk->fops = NULL;
    ramblock_disk->queue = ramblock_queue;
    snprintf(ramblock_disk->disk_name, 32, "ramblock");
    set_capacity(ramblock_disk, RAMBLOCK_NUM_SECTORS);
    add_disk(ramblock_disk);

    printk(KERN_INFO "ram_block: Module loaded.\n");
    return 0;
}

static void __exit ramblock_exit(void)
{
    del_gendisk(ramblock_disk);
    put_disk(ramblock_disk);
    blk_cleanup_queue(ramblock_queue);
    blk_mq_free_tag_set(&tag_set);
    unregister_blkdev(major_num, DEVICE_NAME);
    vfree(ramblock_data);
    printk(KERN_INFO "ram_block: Module unloaded.\n");
}

module_init(ramblock_init);
module_exit(ramblock_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("RAM-backed Block Device Driver");
