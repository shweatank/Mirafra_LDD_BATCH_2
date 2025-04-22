#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
int counter=0;
pthread_t t[2];
#define num 10
void *func(void *arg)
{
	printf("increment counter=%d\n",++counter);
}

void *func1(void *arg)
{
	printf("decrement counter=%d\n",--counter);
}

int main()
{
	for(int i=0;i<num;i++)
	{
	pthread_create(&t[i],NULL,&func,NULL);
	pthread_create(&t[i],NULL,&func1,NULL);
	}

	for(int i=0;i<2;i++)
	{
	pthread_join(t[i],NULL);
	}
}

