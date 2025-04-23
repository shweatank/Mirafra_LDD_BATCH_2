#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
int data=0;
int readcount=0;
sem_t wrt,mutex;
void *writter()
{
		sem_wait(&wrt);
		data+=10;
		printf("write data: %d\n",data);
		sem_post(&wrt);
}
void *reader()
{
		//sleep(1);    //it will 
		sem_wait(&mutex);
		readcount++;
		if(readcount==1)
		{
				sem_wait(&wrt);
		}
				sem_post(&mutex);

				//critical section reading
				printf("read data: %d\n",data);
				sem_wait(&mutex);
				readcount--;
				if(readcount==0)
				{
						sem_post(&wrt);
				}
						sem_post(&mutex);
}
int main()
{
		pthread_t t1,t2,t3;
		sem_init(&mutex,0,1);
		sem_init(&wrt,0,1);
		pthread_create(&t1,NULL,reader,NULL);
		pthread_create(&t2,NULL,writter,NULL);
		pthread_join(t2,NULL);
		pthread_create(&t3,NULL,reader,NULL);
		pthread_join(t1,NULL);
		//pthread_join(t2,NULL);
		pthread_join(t3,NULL);
		sem_destroy(&mutex);
		sem_destroy(&wrt);
}
