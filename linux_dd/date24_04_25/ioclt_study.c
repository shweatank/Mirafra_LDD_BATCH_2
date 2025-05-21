#



#define DEVICE_NAME "ioctl_study"
#define BUF_LEN 100;

#define MAGIC_NUM 100
#define IOCTl_SET_DATA _IOW(MAGIC_NUM,0,char*);
#define IOCTl_GET_RESULT _IOR(MAGIC_NUM,0,char*);

static int dev_open(struct inode * inode ,struct file * file );
static int dev_relese(struct inode * node ,struct file * file);
//static int dev_read(struct file * file ,const char __user * buffer, ssize_t len,loff_t * offset);
//static int dev_read(struct file * file ,const char __user * buffer, ssize_t len,loff_t * offset);
static long dev_ioctl(struct  file *file,unsigned int cmd ,unsigned long arg );


static int dev_open(struct inode * inode ,struct file * file ){
	printk(KERN_INFO"opend the file\n");
	return 0;

}

static int dev_release(struct inode * inode ,struct file * file ){
	printk(KERN_INFO"released the file\n");
	return 0;

}

static int dev_ioctl(struct file *file,unsigned long arg){

size_t len,i;
switch(cmd){



	case IOCTL_SET_DATA:
		if(copy_from_user(message,(char __user*)arg,BUF_LEN))
		 	return -EFAULT;
		 	
		 	message[BUF_LEN - 1]='\0';
		 	len =strnlen(message,BUF_LEN);
		 	
		
		
		
		
}


return 0;
}





static struct file_operations fops ={
	.unlocked_ioctl =dev-ioctl,
	.open =dev_open,
	.release=dev_release};

static int __init ioctl_study_init(void){
	major =register_chardev(0,DEVICE_NAME,&fops);
	if(major<0){
	
		printf(KERN_INFO"ioctel_styday:failed major number");
		return major;
		}
		
	printk(KERN_INFO"IOCTEL_STUDY:MAJOR NUMBER %d\n",major);

return 0;
}


static void __exit ioctl_study_exit(void){
	unregister_chrdev(major,DEVICE_NAME)


}
