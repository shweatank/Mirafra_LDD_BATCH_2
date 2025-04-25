#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>


#define DEVICE_NAME  "charstr"
#define BUF_LEN      100

static int major;
static char message[BUF_LEN];
//static int result;

void reverse_string( char msg[],int length);
void swap_num( char buf[], int start, int end);
void swap_num( char buf[], int start, int end)
{
	char temp = 0;
	temp =  buf[start];
	buf[start] =  buf[end];
	buf[end] = temp;
}
void reverse_string( char msg[],int length)
{
          int size =  length - 1;
	for(int iter = 0; iter < length/2 ; iter++){
	       swap_num(msg,iter,(size - iter));
	}
	printk(KERN_INFO" reversed string is %s",msg);
}

static int dev_open(struct inode *inodep, struct file *filep) {

	printk(KERN_INFO "charstr:device opened\n");
	return 0;
}
static int dev_release(struct inode *inodep,struct file *filep){
	printk(KERN_INFO "charstr:device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep , char __user * buffer, size_t len,loff_t *ffset){

	char result_msg[BUF_LEN];
	//int msg_len = snprintf(result_msg,BUF_LEN,"Result: %d\n",result);
	int msg_len = snprintf(result_msg,BUF_LEN,"Reversed string is  %s\n",message);
	int error_count = copy_to_user(buffer,result_msg,msg_len);
	return error_count == 0?msg_len: -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset){
      
       	printk(KERN_INFO "charstr: called from user");
	if (copy_from_user(message, buffer, len))
		return -EFAULT;

	message[len] = '\0';

#if 0
	if (sscanf(message, "%d %c %d", &a, &op, &b) != 3) {
		printk(KERN_ERR "charcalc: Invalid input format\n");
		return -EINVAL;
	}

	switch (op) {
		case '+': result = a + b; break;
		case '-': result = a - b; break;
		default:
			  printk(KERN_ERR "charcalc: Invalid operator %c\n", op);
			  return -EINVAL;
	}
	printk(KERN_INFO "charcalc: %d %c %d = %d\n", a, op, b, result);
          
	if (sscanf(message, "%d", &number) != 1) {
		printk(KERN_ERR "charcalc: Invalid input format\n");
		return -EINVAL;
	}
         result = number * number;
	 printk(KERN_INFO "charcalc: square of the number %d = %d\n", number,result);
#endif
	 reverse_string(message,len);
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
		printk(KERN_ALERT "char str failed to register a major number\n");
		return major;
	}
	printk(KERN_INFO "charstr: registered with major number %d\n", major);
	return 0;
}

static void __exit charcalc_exit(void) {
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO "charstr: unregistered\n");
}

module_init(charcalc_init);
module_exit(charcalc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ganga");
MODULE_DESCRIPTION("A simple character device that performs reverse a string");
