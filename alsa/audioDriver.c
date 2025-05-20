#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "audio_dev"
#define BUF_SIZE 4096

static char audio_buf[BUF_SIZE];
static int major;
static int read_ptr = 0, write_ptr = 0;

static ssize_t audio_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
	int bytes = min(len, (size_t)(write_ptr - read_ptr));
	if (bytes <= 0) return 0;

	if (copy_to_user(buf, audio_buf + read_ptr, bytes))
		return -EFAULT;

	read_ptr += bytes;
	return bytes;
}

static ssize_t audio_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
	int bytes = min(len, (size_t)(BUF_SIZE - write_ptr));
	if (bytes <= 0) return -ENOSPC;

	if (copy_from_user(audio_buf + write_ptr, buf, bytes))
		return -EFAULT;

	write_ptr += bytes;
	return bytes;
}

static int audio_open(struct inode *inode, struct file *file) {
	pr_info("Audio device opened\n");
	return 0;
}

static int audio_release(struct inode *inode, struct file *file) {
	pr_info("Audio device closed\n");
	return 0;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = audio_read,
	.write = audio_write,
	.open = audio_open,
	.release = audio_release,
};

static int __init audio_init(void) {
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if (major < 0) {
		pr_err("Failed to register character device\n");
		return major;
	}
	pr_info("Audio driver loaded, major = %d\n", major);
	return 0;
}

static void __exit audio_exit(void) {
	unregister_chrdev(major, DEVICE_NAME);
	pr_info("Audio driver unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple Dummy Audio Driver");

module_init(audio_init);
module_exit(audio_exit);
