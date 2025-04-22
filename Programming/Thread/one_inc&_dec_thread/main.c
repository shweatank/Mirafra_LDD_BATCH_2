
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int counter = 0;
pthread_mutex_t lock;

void* increment_thread(void* arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        printf("Inc counter=%d\n", counter);
        pthread_mutex_unlock(&lock);
        usleep(100000);
    }
    return NULL;
}

void* decrement_thread(void* arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&lock);
        counter--;
        printf("Dec counter=%d\n", counter);
        pthread_mutex_unlock(&lock);
        usleep(100000);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    
    pthread_mutex_init(&lock, NULL);
    
    pthread_create(&tid1, NULL, increment_thread, NULL);
    pthread_create(&tid2, NULL, decrement_thread, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_mutex_destroy(&lock);

    printf("after loop completed the value %d\n", counter);

    return 0;
}

