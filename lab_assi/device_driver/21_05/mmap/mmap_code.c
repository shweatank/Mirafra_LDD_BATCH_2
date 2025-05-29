#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>


#define DEVICE_NAME "mmap_dev"
#define BUF_SIZE PAGE_SIZE

static dev_t dev;
static struct cdev mmap_cdev;
static struct class *mmap_class;
static char *kernel_buffer;

static int mmap_open(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "mmap_dev: opened\n");
    return 0;
}

static int mmap_release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "mmap_dev: released\n");
    return 0;
}

static int mmap_mmap(struct file *filp, struct vm_area_struct *vma) {
    unsigned long pfn = virt_to_phys(kernel_buffer) >> PAGE_SHIFT;
    int ret;

    ret = remap_pfn_range(vma,
                          vma->vm_start,
                          pfn,
                          vma->vm_end - vma->vm_start,
                          vma->vm_page_prot);

    if (ret) {
        printk(KERN_ERR "mmap_dev: remap_pfn_range failed\n");
        return ret;
    }

    printk(KERN_INFO "mmap_dev: memory mapped to user space\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mmap_open,
    .release = mmap_release,
    .mmap = mmap_mmap,
};

static int __init my_mmap_init(void) {
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&mmap_cdev, &fops);
    cdev_add(&mmap_cdev, dev, 1);
    mmap_class = class_create(DEVICE_NAME);
    device_create(mmap_class, NULL, dev, NULL, DEVICE_NAME);

    kernel_buffer = (char *)get_zeroed_page(GFP_KERNEL);
    strcpy(kernel_buffer, "Hello from kernel via mmap!\n");

    printk(KERN_INFO "mmap_dev: module loaded\n");
    return 0;
}

static void __exit my_mmap_exit(void) {
    device_destroy(mmap_class, dev);
    class_destroy(mmap_class);
    cdev_del(&mmap_cdev);
    unregister_chrdev_region(dev, 1);
    free_page((unsigned long)kernel_buffer);
    printk(KERN_INFO "mmap_dev: module unloaded\n");
}

module_init(my_mmap_init);
module_exit(my_mmap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("Simple mmap example");

