#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_write = PTHREAD_COND_INITIALIZER;

int readers = 0;          
int writers = 0;          
int waiting_writers = 0; 

void* reader(void* arg) {
    pthread_mutex_lock(&lock);

    // Wait if a writer is writing or waiting
    while (writers > 0 || waiting_writers > 0) {
        printf("Reader is waiting...\n");
        pthread_cond_wait(&can_read, &lock);
    }

    readers++;
    pthread_mutex_unlock(&lock);

   
    printf("Reader is reading...\n");
    sleep(2);  
    printf("Reader finished reading.\n");

    pthread_mutex_lock(&lock);
    readers--;

    if (readers == 0) {
        pthread_cond_signal(&can_write); // wake up writer
    }
    pthread_mutex_unlock(&lock);

    return NULL;
}

void* writer(void* arg) {
    pthread_mutex_lock(&lock);
    waiting_writers++;

    // Wait until no readers or writers
    while (readers > 0 || writers > 0) {
        printf("Writer is waiting...\n");
        pthread_cond_wait(&can_write, &lock);
    }

    waiting_writers--;
    writers++;
    pthread_mutex_unlock(&lock);

   
    printf("Writer is writing...\n");
    sleep(2);  
    printf("Writer finished writing.\n");

    pthread_mutex_lock(&lock);
    writers--;

    if (waiting_writers > 0) {
        pthread_cond_signal(&can_write); // let next writer go
    } else {
        pthread_cond_broadcast(&can_read); // let all readers go
    }

    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    pthread_t t1, t2;

    
    pthread_create(&t1, NULL, reader, NULL);
    sleep(1);  
    pthread_create(&t2, NULL, writer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}

