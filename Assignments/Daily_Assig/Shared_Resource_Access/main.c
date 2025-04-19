#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_read = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_write = PTHREAD_COND_INITIALIZER;

int readers = 0;              
int writers = 0;              
int waiting_writers = 0;      

void *reader(void *arg) {
int id = *((int *)arg);
free(arg);

pthread_mutex_lock(&rw_mutex);
while (writers > 0 || waiting_writers > 0) {
    pthread_cond_wait(&can_read, &rw_mutex);
}
readers++;
pthread_mutex_unlock(&rw_mutex);

printf("reader %d is reading...\n", id);
sleep(1);
printf("reader %d finished reading.\n", id);

pthread_mutex_lock(&rw_mutex);
readers--;
if (readers == 0) {
    pthread_cond_signal(&can_write);
}
pthread_mutex_unlock(&rw_mutex);
return NULL;
}

void *writer(void *arg) {
int id = *((int *)arg);
free(arg);

pthread_mutex_lock(&rw_mutex);
waiting_writers++;
while (readers > 0 || writers > 0) {
    pthread_cond_wait(&can_write, &rw_mutex);
}
waiting_writers--;
writers++;
pthread_mutex_unlock(&rw_mutex);

printf("writer %d is writing...\n", id);
sleep(2);
printf("writer %d finished writing.\n", id);

pthread_mutex_lock(&rw_mutex);
writers--;
if (waiting_writers > 0) {
    pthread_cond_signal(&can_write);
} else {
    pthread_cond_broadcast(&can_read);
}
pthread_mutex_unlock(&rw_mutex);
return NULL;
}

int main() {
pthread_t r[5], w[3];

for (int i = 0; i < 5; i++) {
    int *id = malloc(sizeof(int));
    *id = i + 1;
    pthread_create(&r[i], NULL, reader, id);
    if (i < 3) {
	    int *wid = malloc(sizeof(int));
            *wid = i + 1;
            pthread_create(&w[i], NULL, writer, wid);
        }
    sleep(1);
    }

    for (int i = 0; i < 5; i++) pthread_join(r[i], NULL);
    for (int i = 0; i < 3; i++) pthread_join(w[i], NULL);

    return 0;
}

