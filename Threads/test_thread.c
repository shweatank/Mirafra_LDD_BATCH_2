#include <pthread.h>
#include <stdio.h>

void* foo(void* arg) {
    printf("Thread is running.\n");
    return NULL;
}

int main() {
    pthread_t thread1;
    pthread_create(&thread1, NULL, foo, NULL);

    // Wait for thread to finish
    pthread_join(thread1, NULL);

    return 0;
}

