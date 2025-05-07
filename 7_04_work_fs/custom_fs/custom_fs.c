#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/kernel.h>
#include <linux/mount.h>      // For mnt_idmap()
#include <linux/time64.h>     // For current_time()
#include <linux/user_namespace.h>

#define MYFS_MAGIC 0x64668735

static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
    struct inode *inode = new_inode(sb);

    if (!inode)
        return NULL;

    // Initialize inode owner with correct user namespace
    inode_init_owner( inode, NULL, mode); // Fixed argument

    inode->i_ino = get_next_ino();
    inode->i_sb = sb;
    inode->i_op = &simple_dir_inode_operations;
    inode->i_fop = &simple_dir_operations;

    // Set timestamps using current_time
    inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);

    return inode;
}

static int myfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *root;

    sb->s_magic = MYFS_MAGIC;

    static const struct super_operations myfs_super_ops = {
        .statfs = simple_statfs,
        .drop_inode = generic_delete_inode,
    };

    sb->s_op = &myfs_super_ops;

    root = myfs_make_inode(sb, S_IFDIR | 0755);
    if (!root)
        return -ENOMEM;

    struct dentry *root_dentry = d_make_root(root);
    if (!root_dentry)
        return -ENOMEM;

    sb->s_root = root_dentry;
    return 0;
}

static struct dentry *myfs_mount(struct file_system_type *fs_type,
                    int flags, const char *dev_name, void *data)
{
    return mount_nodev(fs_type, flags, data, myfs_fill_super);
}

static void myfs_kill_sb(struct super_block *sb)
{
    kill_litter_super(sb);
}

static struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name = "myfs",
    .mount = myfs_mount,
    .kill_sb = myfs_kill_sb,
    .fs_flags = FS_USERNS_MOUNT,
};

static int __init myfs_init(void)
{
    return register_filesystem(&myfs_type);
}

static void __exit myfs_exit(void)
{
    unregister_filesystem(&myfs_type);
}

module_init(myfs_init);
module_exit(myfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("A simple custom filesystem example");
