#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void *print_message_function(void *ptr);

 main(){
	pthread_t thread1,thread2;
	char *mess1="thread 1";
	char *mess2="thread 2";

	int  iret1=pthread_create(&thread1,NULL,&print_message_function,(void*) mess1);
	 int iret2=pthread_create(&thread2,NULL,&print_message_function,(void*) mess2);

	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

	exit(0);
}

void *print_message_function(void *ptr)
{
	char *message;
	message=(char *) ptr;
	printf("%s\n",message);
}
