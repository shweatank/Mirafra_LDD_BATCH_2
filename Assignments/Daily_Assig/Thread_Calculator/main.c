#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

typedef struct {
    int num1, num2, result;
    char op;
    int input_ready;
    int result_ready;
    int quit_flag;
} SharedData;

SharedData data = {0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_input = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_result = PTHREAD_COND_INITIALIZER;

///////for input///////////////////////
void* input_thread_func(void* arg) {
    char input[100];

    while (1) {
        pthread_mutex_lock(&mutex);

        while (data.input_ready)
            pthread_cond_wait(&cond_result, &mutex);

        printf("enter expression");
        fflush(stdout);
        fgets(input, sizeof(input), stdin);

        if (input[0] == 'q' || input[0] == 'Q') {
            data.quit_flag = 1;
            data.input_ready = 1;
            pthread_cond_signal(&cond_input);
            pthread_mutex_unlock(&mutex);
            break;
        }

        if (sscanf(input, "%d %c %d", &data.num1, &data.op, &data.num2) != 3) {
            printf("invalid format\n");
            pthread_mutex_unlock(&mutex);
            continue;
        }

        data.input_ready = 1;
        pthread_cond_signal(&cond_input);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

///////////for calc///////////////////////
void* calculator_thread_func(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (!data.input_ready)
            pthread_cond_wait(&cond_input, &mutex);

        if (data.quit_flag) {
            data.result_ready = 1;
            pthread_cond_signal(&cond_result);
            pthread_mutex_unlock(&mutex);
            break;
        }

        switch (data.op) {
            case '+': data.result = data.num1 + data.num2; break;
            case '-': data.result = data.num1 - data.num2; break;
            case '*': data.result = data.num1 * data.num2; break;
            case '/':
                if (data.num2 == 0) {
                    printf("division by zero\n");
                    data.result = 0;
                } else {
                    data.result = data.num1 / data.num2;
                }
                break;
            default:
                printf("unknown op: %c\n", data.op);
                data.result = 0;
        }

        data.input_ready = 0;
        data.result_ready = 1;
        pthread_cond_signal(&cond_result);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

//////for  write//////////////////////
void* writer_thread_func(void* arg) {
    int fd = open("results.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&mutex);

        while (!data.result_ready)
            pthread_cond_wait(&cond_result, &mutex);

        if (data.quit_flag) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        char buffer[100];
        int len = snprintf(buffer, sizeof(buffer), "%d %c %d = %d\n",
                           data.num1, data.op, data.num2, data.result);

        if (write(fd, buffer, len) == -1) {
            perror("write");
        } else {
            printf("saved %s", buffer);
        }

        data.result_ready = 0;
        pthread_cond_signal(&cond_result); 
        pthread_mutex_unlock(&mutex);
    }

    close(fd);
    return NULL;
}

int main() {
    pthread_t t_input, t_calc, t_writer;

    pthread_create(&t_input, NULL, input_thread_func, NULL);
    pthread_create(&t_calc, NULL, calculator_thread_func, NULL);
    pthread_create(&t_writer, NULL, writer_thread_func, NULL);

    pthread_join(t_input, NULL);
    pthread_join(t_calc, NULL);
    pthread_join(t_writer, NULL);

    printf("saved in results.txt\n");
    return 0;
}

