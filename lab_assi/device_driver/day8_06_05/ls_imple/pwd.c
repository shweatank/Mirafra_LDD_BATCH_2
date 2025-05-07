#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/path.h>
#include <linux/sched.h>
#include <linux/slab.h>

static int __init pwd_init(void)
{
    char *buf;
    char *cwd;
    struct path pwd_path;

    // Get current task's fs struct (this is still accessible)
    struct fs_struct *fs = current->fs;

    if (!fs) {
        printk(KERN_ERR "pwd: current->fs is NULL\n");
        return -EFAULT;
    }

    // Take reference to the current working directory
    
    pwd_path = fs->pwd;
    path_get(&pwd_path);   // Increment ref count


    // Allocate memory for output
    buf = kmalloc(PATH_MAX, GFP_KERNEL);
    if (!buf) {
        path_put(&pwd_path);
        return -ENOMEM;
    }

    cwd = d_path(&pwd_path, buf, PATH_MAX);
    if (!IS_ERR(cwd)) {
        printk(KERN_INFO "Current working directory: %s\n", cwd);
    } else {
        printk(KERN_ERR "d_path failed\n");
    }

    kfree(buf);
    path_put(&pwd_path);  // Release reference
    return 0;
}

static void __exit pwd_exit(void)
{
    printk(KERN_INFO "pwd module unloaded\n");
}

module_init(pwd_init);
module_exit(pwd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Working directory print kernel module");

