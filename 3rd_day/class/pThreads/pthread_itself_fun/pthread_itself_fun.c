#include <stdio.h>
#include <pthread.h>

#define NTHREADS 100
void *thr_fun(void *);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int c =0;
int main()
{
	pthread_t thread_id[NTHREADS];
	int i,j;
	for(i = 0;i<NTHREADS; i++)
	{
		pthread_create(&thread_id[i], NULL, thr_fun, NULL);
	}

	for(j=0;j<NTHREADS; j++)
	{
		pthread_join(thread_id[i], NULL);
	}
	
	printf("Final counter value : %d\n", c);	
}

void *thr_fun(void *args)
{
	printf("Threads number %ld\n", pthread_self());
	printf("Counter %d\n", c);
	c++;

}
