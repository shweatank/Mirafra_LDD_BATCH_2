#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>


// Log levels
#define LOG_DEBUG   0
#define LOG_INFO    1
#define LOG_WARNING 2
#define LOG_ERROR   3
// Default log level
#define LOG_LEVEL   LOG_ERROR
#define TRACE_BUFFSIZE  512
#define DEVICE_NAME  "driverfile"

static char trace_buffer[TRACE_BUFFSIZE];
static int major;
static char message[100];
static int write_offset;
static int current_log_level;

void reverse_string( char msg[],int length);
void swap_num( char buf[], int start, int end);
static const char *log_level_to_string(int level);
void log_message(int level, const char *format);
void set_log_level(int level);

int cmdlogLevel = 0;
module_param(cmdlogLevel, int, 0644);
MODULE_PARM_DESC(cmdlogLevel, "Logging level for command logging");

#if 0
int mystrlen(const char *string);
int mystrlen(const char *string) {
	if (string == NULL)
		return 0;

	const char *ptr = string;
	while (*ptr != '\0') {
		ptr++;
	}
	return (ptr - string);
}
#endif

static const char *log_level_to_string(int level) {
	switch (level) {
		case LOG_DEBUG:   return "DEBUG";
		case LOG_INFO:    return "INFO";
		case LOG_WARNING: return "WARNING";
		case LOG_ERROR:   return "ERROR";
		default:          return "UNKNOWN";
	}
}

// Function to set the log level
void set_log_level(int level) {
	current_log_level = level;
}
#if 0
void log_message(int level, const char *format) {
	if (level < void set_log_buffer(char *buffer) {
			trace_buffer = buffer;
#endif
			void log_message(int level, const char *data) 
			{
			if (level < current_log_level) {
			return; // Don't log if the level is lower than the current level
			}

			int length = 0; 
			const char *level_str = log_level_to_string(level);      
			printk(KERN_INFO "Ganga k printing my logs\n");
#if 0
			int len = snprintf(trace_buffer,strlen(level_str),"[%s]",level_str);
			snprintf(trace_buffer+len,TRACE_BUFFSIZE - 2 - len,format);
			snprintf(trace_buffer,sizeof("\n"), "\n");
#else
			length = snprintf(&trace_buffer[write_offset], TRACE_BUFFSIZE - write_offset,"[%s] ",level_str);
			write_offset += length;
			length = snprintf(&trace_buffer[write_offset],TRACE_BUFFSIZE - write_offset,"%s",data);
			write_offset += length;
			length = snprintf(&trace_buffer[write_offset],2,"%s","\n");
			write_offset += length;
			write_offset = write_offset % TRACE_BUFFSIZE;                
#endif	    

			}	

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
				log_message(LOG_DEBUG," string reversed");
				log_message(LOG_INFO," GANGA string reversed");
			}

			static int dev_open(struct inode *inodep, struct file *filep) {

				printk(KERN_INFO "charstr:device opened\n");
				log_message(LOG_DEBUG,"DEVICE DRIVER OPENED");
				return 0;
			}
			static int dev_release(struct inode *inodep,struct file *filep){
				printk(KERN_INFO "charstr:device closed\n");
				log_message(LOG_DEBUG,"DEVICE DRIVER CLOSED");
				return 0;
			}

			static ssize_t dev_read(struct file *filep , char __user * buffer, size_t len,loff_t *ffset){

				int error_count = -1;
#if 0
				char result_msg[BUF_LEN];
				//int msg_len = snprintf(result_msg,BUF_LEN,"Result: %d\n",result);
				int msg_len = snprintf(result_msg,BUF_LEN,"Reversed string is  %s\n",message);
				int error_count = copy_to_user(buffer,result_msg,msg_len);
				return error_count == 0?msg_len: -EFAULT;
#endif  
				printk(KERN_INFO"write_offset = %d",write_offset);
				if(write_offset){
					error_count = copy_to_user(buffer,trace_buffer,write_offset);
					return error_count == 0 ? write_offset : -EFAULT;
				} else {
					return error_count;
				}
			}

			static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset){

				printk(KERN_INFO "driverfile: called from user");
				log_message(LOG_DEBUG," GOT DATA FROM USER");
				log_message(LOG_INFO," GOT DATA FROM USER");
				if (copy_from_user(message, buffer, len))
					return -EFAULT;

				message[len] = '\0';
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
					log_message(LOG_DEBUG,"registered major number failed");
					return major;
				}
				set_log_level(cmdlogLevel);
				printk(KERN_INFO "cmdlogLevel =  %d\n", cmdlogLevel);
				printk(KERN_INFO "charstr: registered with major number %d\n", major);
				log_message(LOG_DEBUG,"major number created");
				return 0;
			}

			static void __exit charcalc_exit(void) {
				unregister_chrdev(major, DEVICE_NAME);
				printk(KERN_INFO "charstr: unregistered\n");
				log_message(LOG_DEBUG,"exit the character string module");
			}

			module_init(charcalc_init);
			module_exit(charcalc_exit);

			MODULE_LICENSE("GPL");
			MODULE_AUTHOR("Ganga");
			MODULE_DESCRIPTION("A simple character device that performs reverse a string and added custom logs");
