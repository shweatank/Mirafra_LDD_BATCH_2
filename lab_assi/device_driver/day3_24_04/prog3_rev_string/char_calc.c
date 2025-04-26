#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>


#define DEVICE_NAME "charcalc"
#define BUF_LEN 100

static int major;
static char message[BUF_LEN];
static char name[50];

static int dev_open(struct inode *inodep,struct file *filep){
	printk(KERN_INFO "charcalc:DEvice opened\n");
	return 0;
}
static int dev_release(struct inode *inodep,struct file *filep){
	printk(KERN_INFO "charcalc:DEvice closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep,char __user *buffer,size_t len, loff_t *offset){
	char result_msg[BUF_LEN];
	int msg_len=snprintf(result_msg,BUF_LEN,"reverse string: %s\n",name);
	int error_count=copy_to_user(buffer,result_msg,msg_len);

	return error_count==0 ? msg_len : -EFAULT;
}

int count_length_string(char *p){
	int count=0;
	while(*p!='\0'){
	 count++;
	 p++;
	 }
	 return count;
}

void reversed_operation(char name[],int z){
	char temp;
	for(int i=0;i<z/2;i++){
    	        temp=name[i];
    		name[i]=name[z-1-i];
    		name[z-1-i]=temp;
    	}
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    
    //char name[50];
  //  int z=strlen(name);
    if (copy_from_user(message, buffer, len))
        return -EFAULT;

    message[len] = '\0';
  //  if (sscanf(message, "%s", name) != 1) {
    //    printk(KERN_ERR "Invalid input format\n");
      //  return -EINVAL;
    //}
    strncpy(name, message, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    int z=count_length_string(name);
    reversed_operation(name,z);
    printk(KERN_INFO "reverse of string is %s\n",name);
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init charcalc_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "reversed string to register a major number\n");
        return major;
    }
    printk(KERN_INFO "reversed string registered with major number %d\n", major);
    return 0;
}

static void __exit charcalc_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "reversed string: unregistered\n");
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("A simple character device that performs + and -");













