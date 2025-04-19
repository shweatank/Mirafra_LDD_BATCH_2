/********************************

2. Shared Resource Access with Reader/Writer Threads

********************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>    
#include <unistd.h>   
#include <string.h> 

int data;

pthread_mutex_t r_lock;
pthread_mutex_t w_lock;

void *reader(void *arg){
    int id = *(int *)arg;
    while(1){
        pthread_mutex_lock(&r_lock);
        
        printf("Reader %d reads %d\n",id,data);
        pthread_mutex_unlock(&r_lock);
        sleep(1);
    }
    
    return NULL;
}
void *writer(void *arg){
    int id = *(int *)arg;
    while(1)
    {
        pthread_mutex_lock(&w_lock);
        //if(data != prev_data)
        {
            data += 1;
            printf("Writer %d writes %d\n",id,data);
        }
        pthread_mutex_unlock(&w_lock);
        sleep(1);
    }
    
    return NULL;
}

int main()
{
    pthread_t r_thread[2], w_thread[2];
    int r_id[2],w_id[2];
    
    for (int i = 0; i < 2; i++) {
        r_id[i] = i + 1;
        pthread_create(&r_thread[i], NULL, reader, &r_id[i]);
    }
    for (int i = 0; i < 2; i++) {
        w_id[i] = i + 1;
        pthread_create(&w_thread[i], NULL, writer, &w_id[i]);
    }
    for (int i = 0; i < 2; i++)
        pthread_join(r_thread[i], NULL);
    for (int i = 0; i < 2; i++)
        pthread_join(w_thread[i], NULL);
    return 0;
}
