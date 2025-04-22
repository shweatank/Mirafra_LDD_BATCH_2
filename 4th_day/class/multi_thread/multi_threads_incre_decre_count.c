#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
int counter =1;

pthread_mutex_t mutex;

void *increment(void *args)
{
	while(1){
	int id = *(int *)args;
	pthread_mutex_lock(&mutex);
	counter++;
	printf("Thread %d is incrementing counter value %d\n",id, counter);
	pthread_mutex_unlock(&mutex);
	sleep(3);
	}
}

void *decrementing(void *args)
{
	while(1){
	int id = *(int *)args;
        pthread_mutex_lock(&mutex);
        counter--;
        printf("Thread %d is decrementing counter value %d\n", id, counter);
        pthread_mutex_unlock(&mutex);
	sleep(3);
	}

}
int main()
{
	pthread_t t1, t2;
	int i1=1, i2=2;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&t1, NULL, increment, (void *)&i1);
	pthread_create(&t2, NULL, decrementing,(void *)&i2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
