#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NTHREADS 100
void *thread_function(void *);
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

int main()
{
  pthread_t thread_id[NTHREADS];
  int i,j;

  for(i=0;i<NTHREADS;i++)
  {
    pthread_create(&thread_id[i],NULL,thread_function,NULL);

  }

  for(j=0;j<NTHREADS;j++)
  {
    pthread_join(thread_id[j],NULL);
  }


  printf("Final conter value: %d\n",counter);

}

void *thread_function(void *dummyptr)
{
  printf("Thread number %d\n",pthread_self());
  printf("Counter %d\n",counter);

  counter++;
}
