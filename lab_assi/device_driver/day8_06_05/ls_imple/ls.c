#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/dcache.h>
#include <linux/slab.h>

struct my_ls_context {
    struct dir_context ctx;
};

static int ls_filldir(struct dir_context *ctx, const char *name, int namlen,
                      loff_t offset, u64 ino, unsigned int d_type)
{
    pr_info("ls: %.*s\n", namlen, name);
    return 0;
}

static int __init ls_init(void)
{
    struct path path;
    struct file *dir;
    struct my_ls_context ls_ctx = {
        .ctx.actor = ls_filldir,
    };

    const char *target_path = "/home/mirafra/lab_assi/device_driver/day8_06_05/ls_imple/";  // Change if needed

    if (kern_path(target_path, LOOKUP_FOLLOW, &path)) {
        pr_err("ls: Failed to resolve path: %s\n", target_path);
        return -ENOENT;
    }

    dir = dentry_open(&path, O_RDONLY, current_cred());
    if (IS_ERR(dir)) {
        pr_err("ls: Failed to open directory: %s\n", target_path);
        path_put(&path);
        return PTR_ERR(dir);
    }

    if (!dir->f_op || !dir->f_op->iterate_shared) {
        pr_err("ls: iterate_shared not supported\n");
        fput(dir);
        path_put(&path);
        return -ENOTDIR;
    }

    pr_info("ls: Contents of %s:\n", target_path);
    dir->f_op->iterate_shared(dir, &ls_ctx.ctx);

    fput(dir);
    path_put(&path);
    return 0;
}

static void __exit ls_exit(void)
{
    pr_info("ls: Kernel module unloaded\n");
}

module_init(ls_init);
module_exit(ls_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Safe ls implementation using iterate_shared");

