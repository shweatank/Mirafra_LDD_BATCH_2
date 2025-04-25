#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ts.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "simple calculator"
#define MAGIC_NUM 100
#define IOCTL_SET_DATA _LOW(MAGIC_NUM, 0, struct calc_data *)
#define IOCTL_GET_RESULT _IOR(MAGIC_NUM, 1, INT *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akash");
MODULE_DESCRIPTION("Kernal calculator using IOCTL AND KMALLOC");
MODULE_VERSION("1.0");

struct calc_data {
int num1;
int num2;
chat operation;
int result;
};

static int major;
static struct calc_data *calc_buffer;

static void perform calculation(void){
switch (calc_buffer->operation){
case '+': calc_buffer->result = calc_buffer->num1 + calc_buffer->num2; break;
case '-': calc_buffer->result = calc_buffer->num1 - calc_buffer->num2; break;
case '*': calc_buffer->result = calc_buffer->num1 * calc_buffer->num2; break;
case '/':
if (calc_buffer->num2 !=0)
calc_buffer->result = calc_buffer->num1 / calc_buffer->num2;
else
calc_buffer->result = 0;
break;
default : calc_buffer->result = 0;
}
}



