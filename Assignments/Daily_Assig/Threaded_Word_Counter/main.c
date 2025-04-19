#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_LINE_LEN 1024
#define QUEUE_SIZE   10
#define NUM_WORKERS  3

typedef struct {
    char* lines[QUEUE_SIZE];
    int front, rear, count;
    int done;
} Queue;

Queue queue = {.front = 0, .rear = 0, .count = 0, .done = 0};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_nonfull = PTHREAD_COND_INITIALIZER;

int total_word_count = 0;
pthread_mutex_t total_mutex = PTHREAD_MUTEX_INITIALIZER;

void enqueue(char* line) {
    queue.lines[queue.rear] = line;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.count++;
}

char* dequeue() {
    char* line = queue.lines[queue.front];
    queue.front = (queue.front + 1) % QUEUE_SIZE;
    queue.count--;
    return line;
}

int count_words(const char* line) {
    int count = 0, in_word = 0;
    for (int i = 0; line[i]; i++) {
        if ((line[i] != ' ') && (line[i] != '\n') && (line[i] != '\t')) {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
    }
    return count;
}
///// for read////
void* reader_thread(void* arg) {
    FILE* file = fopen("input.txt", "r");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char buffer[MAX_LINE_LEN];
    while (fgets(buffer, sizeof(buffer), file)) {
        pthread_mutex_lock(&mutex);
        while (queue.count == QUEUE_SIZE)
            pthread_cond_wait(&cond_nonfull, &mutex);

        enqueue(strdup(buffer));
        pthread_cond_signal(&cond_nonempty);
        pthread_mutex_unlock(&mutex);
    }

    fclose(file);

    pthread_mutex_lock(&mutex);
    queue.done = 1;
    pthread_cond_broadcast(&cond_nonempty);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* worker_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (queue.count == 0 && !queue.done)
            pthread_cond_wait(&cond_nonempty, &mutex);

        if (queue.count == 0 && queue.done) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        char* line = dequeue();
        pthread_cond_signal(&cond_nonfull);
        pthread_mutex_unlock(&mutex);

        int words = count_words(line);
        free(line);

        pthread_mutex_lock(&total_mutex);
        total_word_count += words;
        pthread_mutex_unlock(&total_mutex);
    }
    return NULL;
}

int main() {
    pthread_t reader;
    pthread_t workers[NUM_WORKERS];

    pthread_create(&reader, NULL, reader_thread, NULL);
    for (int i = 0; i < NUM_WORKERS; i++)
        pthread_create(&workers[i], NULL, worker_thread, NULL);

    pthread_join(reader, NULL);
    for (int i = 0; i < NUM_WORKERS; i++)
        pthread_join(workers[i], NULL);

    printf("total word count: %d\n", total_word_count);
    return 0;
}

