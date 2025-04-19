#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
int c=0;
void *thr_fun(void *args)
{
	pthread_mutex_lock(&mutex);
	c++;
	printf("Counter value %d\n", c);
	pthread_mutex_unlock(&mutex);
}
int main()
{
	pthread_t t1, t2;
	int rc1, rc2;
	pthread_mutex_init(&mutex, NULL);

	if((rc1 = pthread_create(&t1, NULL, thr_fun, NULL)))
	{
		printf("Thread creation failed %d\n", rc1);
	}

	if((rc2 = pthread_create(&t2, NULL, thr_fun, NULL)))
	{
		printf("Thread creation failed %d\n", rc2);
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
			
}
