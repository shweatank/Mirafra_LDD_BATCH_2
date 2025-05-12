#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/dirent.h>

#define PROC_NAME "mp4_list"
#define MAX_FILES 1024
#define MAX_PATH 256

static char *mp4_files[MAX_FILES];
static int file_count = 0;
static struct proc_dir_entry *proc_entry;

struct mp4_iterate_ctx {
    struct dir_context ctx;
    const char *base_path;
};

// Forward declaration to avoid implicit declaration
static void search_mp4_files(const char *path);

static bool mp4_filldir(struct dir_context *ctx, const char *name, int namlen,
                        loff_t offset, u64 ino, unsigned int d_type)
{
    struct mp4_iterate_ctx *mp4_ctx = container_of(ctx, struct mp4_iterate_ctx, ctx);
    char *full_path;

    if (d_type == DT_REG) {
        if (namlen > 4 && strcmp(name + namlen - 4, ".mp4") == 0) {
            full_path = kmalloc(MAX_PATH, GFP_KERNEL);
            if (!full_path) {
                printk(KERN_WARNING "Failed to allocate memory for path\n");
                return true; // Continue iteration despite allocation failure
            }
            snprintf(full_path, MAX_PATH, "%s/%s", mp4_ctx->base_path, name);
            if (file_count < MAX_FILES) {
                mp4_files[file_count++] = full_path;
            } else {
                kfree(full_path);
                printk(KERN_WARNING "Max file limit reached\n");
            }
        }
    } else if (d_type == DT_DIR && strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
        char *new_path = kmalloc(MAX_PATH, GFP_KERNEL);
        if (!new_path) {
            printk(KERN_WARNING "Failed to allocate memory for new path\n");
            return true; // Continue iteration
        }
        snprintf(new_path, MAX_PATH, "%s/%s", mp4_ctx->base_path, name);
        search_mp4_files(new_path);
        kfree(new_path);
    }
    return true; // Continue iteration
}

static void search_mp4_files(const char *path)
{
    struct path p;
    struct inode *inode;
    struct file *file;
    struct mp4_iterate_ctx mp4_ctx = {
        .ctx.actor = mp4_filldir,
        .base_path = path,
    };

    if (kern_path(path, 0, &p)) {
        printk(KERN_WARNING "Failed to access path: %s\n", path);
        return;
    }

    inode = p.dentry->d_inode;
    if (!S_ISDIR(inode->i_mode)) {
        path_put(&p);
        return;
    }

    file = filp_open(path, O_RDONLY | O_DIRECTORY, 0);
    if (IS_ERR(file)) {
        path_put(&p);
        printk(KERN_WARNING "Failed to open directory: %s\n", path);
        return;
    }

    iterate_dir(file, &mp4_ctx.ctx);
    filp_close(file, NULL);
    path_put(&p);
}

static int proc_show(struct seq_file *m, void *v)
{
    int i;
    for (i = 0; i < file_count; i++) {
        if (mp4_files[i])
            seq_printf(m, "%s\n", mp4_files[i]);
    }
    return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init mp4_list_init(void)
{
    int i;
    for (i = 0; i < MAX_FILES; i++)
        mp4_files[i] = NULL;
    file_count = 0;

    proc_entry = proc_create(PROC_NAME, 0, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    search_mp4_files("/home");
    printk(KERN_INFO "MP4 list module loaded, found %d files\n", file_count);
    return 0;
}

static void __exit mp4_list_exit(void)
{
    int i;
    for (i = 0; i < file_count; i++) {
        if (mp4_files[i])
            kfree(mp4_files[i]);
    }
    if (proc_entry)
        proc_remove(proc_entry);
    printk(KERN_INFO "MP4 list module unloaded\n");
}

module_init(mp4_list_init);
module_exit(mp4_list_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash");
MODULE_DESCRIPTION("Kernel module to list MP4 files");
