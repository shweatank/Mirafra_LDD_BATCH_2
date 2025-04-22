#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int read_count = 0;
int write_waiting = 0;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;     // reader count & condition access
pthread_mutex_t mutex_rw = PTHREAD_MUTEX_INITIALIZER;   // resource lock
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* read_thread(void* );
void* write_thread(void* );


int main() {
    int size = 3;
    pthread_t r_threads[size], w_threads[size];

    for (int i=0; i<size; i++) {
        pthread_create(&r_threads[i], NULL, read_thread, (void*)i);
    }
    for (int i=0; i<size; i++) {
        pthread_create(&w_threads[i], NULL, write_thread, (void*)i);
    }

    for (int i=0; i<size; i++) {
        pthread_join(r_threads[i], NULL);
    }

    for (int i=0; i<size; i++) {
        pthread_join(w_threads[i], NULL);
    }

    return 0;
}

void* read_thread(void* arg) {
   int read_id = arg;

    pthread_mutex_lock(&mutex1);
    while (write_waiting > 0) {
        pthread_cond_wait(&cond, &mutex1);
    }

    read_count++;
    if (read_count == 1) {
        pthread_mutex_lock(&mutex_rw);
    }
    pthread_mutex_unlock(&mutex1);

    printf("Reader %ld is reading\n", read_id);

    pthread_mutex_lock(&mutex1);
    read_count--;
    if (read_count == 0) {
        pthread_mutex_unlock(&mutex_rw);
    }
    pthread_mutex_unlock(&mutex1);

    return NULL;
}

void* write_thread(void* arg) {
   int write_id =arg;

    pthread_mutex_lock(&mutex1);
    write_waiting++;
    while (read_count > 0 || write_waiting > 1) {
        pthread_cond_wait(&cond, &mutex1);
    }
    write_waiting--;
    pthread_mutex_unlock(&mutex1);

    pthread_mutex_lock(&mutex_rw);
    printf("Writer %d is writing\n", write_id);
    pthread_mutex_unlock(&mutex_rw);

    pthread_cond_broadcast(&cond);  // notify waiting readers/writers

    return NULL;
}


