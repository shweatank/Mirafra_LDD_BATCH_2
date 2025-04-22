#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
pthread_mutex_t lock;
static int count=0;
void *print(void *ptr)
{
	pthread_mutex_lock(&lock);
	count+=1;
	printf("%d",count);
	pthread_mutex_unlock(&lock);
}
int main()
{
	pthread_t t[3];
	int i;
	pthread_mutex_init(&lock,NULL);
	for(i=0;i<3;i++)
	{
		pthread_create(&t[i],NULL,print,NULL);
	}

	for(i=0;i<3;i++)
	{
		pthread_join(t[i],NULL);
	}
	pthread_mutex_destroy(&lock);
}

