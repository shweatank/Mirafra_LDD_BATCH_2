#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function(void * ptr);
int main()
{
	pthread_t thread1, thread2;
	char *msg1 = "Thread 1";
	char *msg2 = "Thread 2";
	int i1, i2;

	i1 = pthread_create(&thread1, NULL , print_message_function, (void *) msg1);
	i2 = pthread_create(&thread2 , NULL, print_message_function, (void *) msg2);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	printf("Thread 1 return : %d\n", i1);
	printf("THread 2 return : %d\n", i2);
	exit(0);
}

void *print_message_function(void *ptr)
{
	char *msg ;
	msg = (char *) ptr;
	printf("%s \n", msg);

}
