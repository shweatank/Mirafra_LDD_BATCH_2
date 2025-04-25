#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/ioctl.h>

#define DEVICE_NAME "/dev/iocal"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _IOW(MAGIC_NUM,0 struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, int *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant");
MODULE_DESCRIPTION("kernel calculator usieng IOCTL and kmalloc");
MODULE_VERSION("1.0");

strcut calc_data{
int num1;
int num2;
char operation;
int result;

};

static int major;
static struct calc_data *calc_buffer;

static void perform_calculation(void)
{
	switch(calc_buffer->operation)
	{
		case '+':calc_buffer->num1 + calc_buffer->num2;
			 break;
                case '-':calc_buffer->num1 - calc_buffer->num2;
                         break;
                case '*':calc_buffer->num1 * calc_buffer->num2;
                         break;
	        case '/':if(calc_buffer->num2 !=0){
                         calc_buffer->result =calc_buffer->num1 / calc_buffer->num2;break;

                         }
                         else
                                 cal_buffer->result=0;
                         break;
                default:calc_buffer->result=0;
        }	 
	
}
static long dev_ioctl(struct file *file.unsigned int cmd,unsigned long arg){

switch (cmd){
        case IOCTL_SET_DATA:if(copy_from_user(calc_buffer,(struct calc_data __user *)arg,sizeof(struct calc_data)))
                                    return -EFAULT;
                            perform_calculation();
                            break;
        case IOCTL_GET_RESULT:if(copy_to_user((int __user *)arg,&calc_buffer->result,sizeof(int)))
                                      return -EFAULT;
                              break;
        default: return -EINVAL;
}
return 0;
}

























