#include<linux/init.h>
#include<linux/module.h>
#include<linux/interrupt.h>
#include<linux/kernel.h>

static int irq =1;
static int counter = 0;
/*
request_irq(irq,function ,flag(IRQF_SHARED),name,DEV(void*));

void irqreturn_t my_irq_fun(int irq,void *dev_id);
free_irq(irq,(void*)&irq)

tasklet :
DECLARE_TASKLET(my_tasklet,mytasklet_function);
tasklet_scheduled(&my_tasklet);
tasklet_kill(&my_tasklet)*/

static void my_tasklet_func(struct tasklet_struct *t)
{
	counter++;
	printk(KERN_INFO"tasklet execution counter = %d \n",counter);
}

DECLARE_TASKLET(my_tasklet,my_tasklet_func);

static irqreturn_t my_irq_handler(int irq, void *dev_id)
{

	tasklet_schedule(&my_tasklet);
	return IRQ_HANDLED;}
	
	
	
	
static int __init my_module_init(void)
{


	int ret ;
	ret =request_irq(irq,my_irq_handler,IRQF_SHARED,"my_tasklet_irq",(void *)&irq);
	if(ret){return ret;}
	return 0;}
	
static void __exit my_module_exit(void)
{
	free_irq(irq,(void*)&irq);
	tasklet_kill(&my_tasklet);
}
	

module_exit(my_module_init);
module_init(my_module_exit);

#include<linux/module.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/kernel.h>
#include<linux/workqueue.h>
#include<linux/slab.h>
#include<linux/string.h>

static int irq =1;
static char *scr_buf = NULL;
static char *dst_buf =NULL;
static int buf_size =128;


static struct work_struct my_work ;
static void work_handler(struct work_struct *work)
{

	snprintf(src_buf,buf_size,"original data[%lu]",jiffies);
	memset(dst_buf,0,buf_size);
	strncpy(dst_buf,scr_buff,buf_size-1);
	strrev(des_buf);
	
}
static irqreturn irq_function(int irq,void dev_id)
{
shedule_work(&my_work);
return IRQ_HANDLER;}
static int __init my_init(void)
{

	int ret;
	scr_buf =kmalloc(buf_size,gpf_)
	INIT_WORK(&my_work,work_handled);
	
	ret=request_irq(irq,irq_function,IRQF_SHARE,"MY_WORK_IRQ",&my_work);}	
	
	
	
	
return 0;	
}	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

