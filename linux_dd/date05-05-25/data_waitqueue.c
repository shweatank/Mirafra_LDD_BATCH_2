/*wit queue diclaration  intialize wait unintialize wait 

dinamic :
	wait_queue_head_t(wait_q);init_waitqueue_head(&wait_q);
	wait_event_intrruptible(wait_q,cond!=0);cont=1;//need to release ->read
	wake_up_interruptable(&wait_q);cond= 0;//cond for release->write	
static:
	DECLARE_WAIT_QUEUE_HEAD(wait_q); 
	wait_event_interruptible(wait_q,condition!=0)condition =1;
	wake_up_interruptible(&wait_q);condition = 0;
	

	*/
