/* Word count in a file by using threads */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_LINE_LEN 100
#define READ_BUF_SIZE 100 
#define QUEUE_CAPACITY 100
#define NUM_WORKERS 4

typedef struct {
    char *buffer[QUEUE_CAPACITY];
    int front, rear, count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
}Queue;

Queue queue;
int total_words = 0;
pthread_mutex_t total_mutex;

void queue_init(Queue *q) {
    q->front = q->rear = q->count = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void queue_push(Queue *q, char *line) {
    pthread_mutex_lock(&q->lock);
    while (q->count == QUEUE_CAPACITY) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    q->buffer[q->rear] = line;
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

char* queue_pop(Queue *q) {
    pthread_mutex_lock(&q->lock);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }
    char *line = q->buffer[q->front];
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);
    return line;
}

int word_count(const char *line) {
    int count = 0, in_word = 0;
    while (*line) {
        if (isspace((unsigned char)*line)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        line++;
    }
    return count;
}

void* reader_func(void *arg) {
    const char *filename = (char *)arg;
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    char read_buf[READ_BUF_SIZE];
    char line_buf[MAX_LINE_LEN];
    
    int line_len = 0;
    int bytes_read;

    while ((bytes_read = read(fd, read_buf, sizeof(read_buf))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (read_buf[i] == '\n' || line_len >= MAX_LINE_LEN - 1) {
                line_buf[line_len] = '\0';
                char *line = strdup(line_buf);
                queue_push(&queue, line);
                line_len = 0;
            } else {
                line_buf[line_len++] = read_buf[i];
            }
        }
    }

    if (line_len > 0) {
        line_buf[line_len] = '\0';
        queue_push(&queue, strdup(line_buf));
    }

    close(fd);

    for (int i = 0; i < NUM_WORKERS; i++) {
        queue_push(&queue, NULL); 
    }

    return NULL;
}

void* worker_func(void *arg) {
    while (1) {
        char *line = queue_pop(&queue);
        if (!line) break;
        int words = word_count(line);
        pthread_mutex_lock(&total_mutex);
        total_words += words;
        pthread_mutex_unlock(&total_mutex);
        free(line);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <inputfile>\n", argv[0]);
        return 1;
    }

    queue_init(&queue);
    pthread_mutex_init(&total_mutex, NULL);

    pthread_t reader, workers[NUM_WORKERS];
    pthread_create(&reader, NULL, reader_func, argv[1]);
  
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_create(&workers[i], NULL, worker_func, NULL);
    }
  
    pthread_join(reader, NULL);
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(workers[i], NULL);
    }

    printf("Total Word Count: %d\n", total_words);

    return 0;
}
