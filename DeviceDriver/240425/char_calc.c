#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "charcale"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static int result;

static int dev_open(struct inode *inodep, struct file *filep){

 printk(KERN_INFO "charcalc: Device opened\n");
 return 0;
 }
 
 static int dev_release(struct inode *inodep, struct file *filep){
 
 printk(KERN_INFO "charcalc: Device closed\n");
 return 0;
 }
 
 static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
 {
 char result_msg[BUF_LEN];
 int msg_len = snprintf(result_msg, BUF_LEN, "Result: %d\n", result);
 int error_count = copy_to_user(buffer, result_msg, msg len);
 
 return error_count == 0? msg_len : -EFAULT;
 }
 
