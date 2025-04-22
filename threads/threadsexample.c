#include <stdio.h>
#include <pthread.h>

int count = 0;

void* increment(void* arg) {
  
        count++;
        printf("Increment thread: count = %d\n", count);
    return NULL;
}

void* decrement(void* arg) 
{
    
        count--;
        printf("Decrement thread: count = %d\n", count);
    return NULL;
}

int main() {
    pthread_t inc_thread, dec_thread;

    pthread_create(&inc_thread, NULL, increment, NULL);
    pthread_create(&dec_thread, NULL, decrement, NULL);

    pthread_join(inc_thread, NULL);
    pthread_join(dec_thread, NULL);

    printf("Final Count: %d\n", count);

    return 0;
}

