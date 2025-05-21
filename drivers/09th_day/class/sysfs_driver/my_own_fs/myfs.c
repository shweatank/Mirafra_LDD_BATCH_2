#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/mount.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/uidgid.h>

#define MYFS_MAGIC 0x13131313
#define MYFS_NAME "myfs"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Custom minimal Linux filesystem driver with inode/dentry logging");

static struct dentry *myfs_mount(struct file_system_type *fs_type,
    int flags, const char *dev_name, void *data);

static void myfs_put_super(struct super_block *sb)
{
    printk(KERN_INFO "myfs: superblock released\n");
}

static int myfs_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "myfs: File opened. inode=%lu\n", inode->i_ino);
    return 0;
}

static struct file_operations myfs_file_ops = {
    .owner = THIS_MODULE,
    .open = myfs_open,
};

static struct super_operations myfs_super_ops = {
    .put_super = myfs_put_super,
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
    struct inode *inode = new_inode(sb);
    if (!inode)
        return NULL;

    inode->i_ino = get_next_ino();
    inode->i_sb = sb;
    inode->i_op = &simple_dir_inode_operations;
    inode->i_fop = &myfs_file_ops;
    inode->i_mode = mode;
    inode->__i_atime = inode->__i_mtime = inode->__i_ctime = current_time(inode);
    inode->i_uid = make_kuid(current_user_ns(), 0);
    inode->i_gid = make_kgid(current_user_ns(), 0);

    return inode;
}

static int myfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *root_inode;

    sb->s_magic = MYFS_MAGIC;
    sb->s_op = &myfs_super_ops;

    root_inode = myfs_make_inode(sb, S_IFDIR | 0755);
    if (!root_inode)
        return -ENOMEM;

    sb->s_root = d_make_root(root_inode);
    if (!sb->s_root)
        return -ENOMEM;

    // Print Inode Info
    printk(KERN_INFO "myfs: Root inode created:\n");
    printk(KERN_INFO "  Inode number: %lu\n", root_inode->i_ino);
    printk(KERN_INFO "  Mode: 0x%x\n", root_inode->i_mode);
    printk(KERN_INFO "  UID: %u, GID: %u\n",
           from_kuid(&init_user_ns, root_inode->i_uid),
           from_kgid(&init_user_ns, root_inode->i_gid));

    // Print Dentry Info
    if (sb->s_root) {
        struct dentry *dentry = sb->s_root;
        printk(KERN_INFO "myfs: Dentry created:\n");
        printk(KERN_INFO "  Dentry name: %s\n", dentry->d_name.name);
        printk(KERN_INFO "  Dentry flags: 0x%x\n", dentry->d_flags);
        printk(KERN_INFO "  Dentry inode: %p\n", dentry->d_inode);
    }

    printk(KERN_INFO "myfs: filesystem mounted with detailed logging\n");
    return 0;
}

static struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name = MYFS_NAME,
    .mount = myfs_mount,
    .kill_sb = kill_litter_super,
};

static struct dentry *myfs_mount(struct file_system_type *fs_type,
    int flags, const char *dev_name, void *data)
{
    return mount_nodev(fs_type, flags, data, myfs_fill_super);
}

static int __init myfs_init(void)
{
    int ret = register_filesystem(&myfs_type);
    printk(KERN_INFO "myfs: registered\n");
    return ret;
}

static void __exit myfs_exit(void)
{
    unregister_filesystem(&myfs_type);
    printk(KERN_INFO "myfs: unregistered\n");
}

module_init(myfs_init);
module_exit(myfs_exit);
