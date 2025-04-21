#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LINE_MAX_LEN 1024
#define BUFFER_CAPACITY 10
#define THREAD_COUNT 3

typedef struct {
    char* buffer[BUFFER_CAPACITY];
    int head, tail, size;
    int is_done;
} LineBuffer;

LineBuffer shared_buffer = {.head = 0, .tail = 0, .size = 0, .is_done = 0};

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_has_data = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_has_space = PTHREAD_COND_INITIALIZER;

int global_word_total = 0;
pthread_mutex_t word_mutex = PTHREAD_MUTEX_INITIALIZER;

// Enqueue a line into the buffer
void buffer_add(char* line) {
    shared_buffer.buffer[shared_buffer.tail] = line;
    shared_buffer.tail = (shared_buffer.tail + 1) % BUFFER_CAPACITY;
    shared_buffer.size++;
}

// Dequeue a line from the buffer
char* buffer_remove() {
    char* line = shared_buffer.buffer[shared_buffer.head];
    shared_buffer.head = (shared_buffer.head + 1) % BUFFER_CAPACITY;
    shared_buffer.size--;
    return line;
}

// Count words in a given string
int compute_word_count(const char* text) {
    int count = 0;
    int inside_word = 0;
    for (int i = 0; text[i]; i++) {
        if (text[i] != ' ' && text[i] != '\n' && text[i] != '\t') {
            if (!inside_word) {
                count++;
                inside_word = 1;
            }
        } else {
            inside_word = 0;
        }
    }
    return count;
}

// Reader thread: reads file and puts lines into buffer
void* file_reader(void* arg) {
    FILE* fp = fopen("input.txt", "r");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }

    char line[LINE_MAX_LEN];
    while (fgets(line, sizeof(line), fp)) {
        pthread_mutex_lock(&buffer_mutex);
        while (shared_buffer.size == BUFFER_CAPACITY)
            pthread_cond_wait(&cond_has_space, &buffer_mutex);

        buffer_add(strdup(line));
        pthread_cond_signal(&cond_has_data);
        pthread_mutex_unlock(&buffer_mutex);
    }

    fclose(fp);

    pthread_mutex_lock(&buffer_mutex);
    shared_buffer.is_done = 1;
    pthread_cond_broadcast(&cond_has_data);
    pthread_mutex_unlock(&buffer_mutex);
    return NULL;
}

// Worker thread: processes lines and counts words
void* word_counter(void* arg) {
    while (1) {
        pthread_mutex_lock(&buffer_mutex);
        while (shared_buffer.size == 0 && !shared_buffer.is_done)
            pthread_cond_wait(&cond_has_data, &buffer_mutex);

        if (shared_buffer.size == 0 && shared_buffer.is_done) {
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }

        char* line = buffer_remove();
        pthread_cond_signal(&cond_has_space);
        pthread_mutex_unlock(&buffer_mutex);

        int word_count = compute_word_count(line);
        free(line);

        pthread_mutex_lock(&word_mutex);
        global_word_total += word_count;
        pthread_mutex_unlock(&word_mutex);
    }
    return NULL;
}

int main() {
    pthread_t reader_thread;
    pthread_t workers[THREAD_COUNT];

    pthread_create(&reader_thread, NULL, file_reader, NULL);
    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_create(&workers[i], NULL, word_counter, NULL);

    pthread_join(reader_thread, NULL);
    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(workers[i], NULL);

    printf("total word count: %d\n", global_word_total);
    return 0;
}

