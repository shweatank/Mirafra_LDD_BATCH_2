#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include <unistd.h>


int read_count=0;
int shared_data=1;
sem_t wrt;           
void *read_t(void *arg)
{
    int id=*(int*)arg;
     read_count++;
     if(read_count==1)
     {
         sem_wait(&wrt);
     }
     printf("Reader %d is reading: %d\n", id, shared_data);
     
     read_count--;
     if(read_count==0)
     {
         sem_post(&wrt);
     }
     
}
void *write_t(void* arg)
{
    int id=*(int*)arg;
    sem_wait(&wrt); 
     shared_data++;
    printf("Writer %d is writing: %d\n", id, shared_data);
    sem_post(&wrt);
}
int main()
{
    pthread_t w_thread[2],r_thread[5];
    sem_init(&wrt, 0, 1);                     
    for(int i=0;i<5;i++)
    {
        pthread_create(&r_thread[i],NULL,read_t,&i);
    }
    for(int i=0;i<2;i++)
    {
         pthread_create(&w_thread[i],NULL,write_t,&i);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(r_thread[i], NULL);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(w_thread[i], NULL);
    }

    sem_destroy(&wrt);
}
