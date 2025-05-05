//spftirq_tasklet_module.c

#include <Linux/module.h>
#include <Linux/kernel.h>
#include <linux/interrupt.h>

//Tasklet function declaration that matches the expected signature

static void my_tasklet_func(struct  tasklet_struct *t);

// Declaration the tasklet (only 2 arguments: name and function)
DECLARE_TASKLET(my_tasklet, ,my_tasklet_func);

//Tasklet handler function with correct signature
static void my_tasklet_func(struct  tasklet_struct *t)
{
    pr_info("Tasklet executed in SoftIRQ context. \n");
}

static int __init tasklet_module_init(void)
{
pr_info("Tasklet module loaded, Scheduling tasklet. \n");
//Schedule the tasklet
tasklet_schedule(&my_tasklet);
return 0;    
}