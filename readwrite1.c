/*
Shared Resource Access with Reader/Writer Threads
Problem:
Simulate a Reader-Writer problem:

Multiple reader threads can read simultaneously.

Writer threads must have exclusive access. Ensure no starvation for writers
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int shareddata = 0;         
int reader_count = 0;        

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;       
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  

void* reader(void* arg) {
    int id = *(int*)arg;

    while (1) {
        
        pthread_mutex_lock(&mutex);
        reader_count++;
        if (reader_count == 1) {
            pthread_mutex_lock(&ock); 
        }
        pthread_mutex_unlock(&mutex);

        
        printf("Reader id %d: read shared data = %d\n", id, shareddata);
        sleep(1);  

        
        pthread_mutex_lock(&mutex);
        reader_count--;
        if (reader_count == 0) {
            pthread_mutex_unlock(&lock); 
        }
        pthread_mutex_unlock(&mutex);

        sleep(2);  
    }

    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;

    while (1) {
        pthread_mutex_lock(&lock);  

        shared_data += 1;
        printf("Writer id %d: wrote to shared data = %d\n", id, shareddata);
        sleep(2); 
        pthread_mutex_unlock(&write_lock); 

        sleep(3); 
    }

    return NULL;
}

int main() {
    pthread_t r1, r2, w1;
    int r_id1 = 1, r_id2 = 2, w_id = 1;

    pthread_create(&r1, NULL, reader, &r_id1);
    pthread_create(&r2, NULL, reader, &r_id2);
    pthread_create(&w1, NULL, writer, &w_id);

    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w1, NULL);

    return 0;
}

