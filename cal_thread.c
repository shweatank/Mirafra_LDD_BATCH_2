#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    double num1;
    double num2;
    char operation;
    double result;
    bool input_ready;
    bool calculation_ready;
    bool done;
    pthread_mutex_t mutex;
    pthread_cond_t input_cond;
    pthread_cond_t calc_cond;
    pthread_cond_t write_cond;
} SharedData;

// Thread fun prototypes
void *input_thread(void *arg);
void *calculator_thread(void *arg);
void *writer_thread(void *arg);

int main()
{
    pthread_t input_tid, calculator_tid, writer_tid;
    SharedData shared_data;

    // Initialize shared data
    shared_data.input_ready = false;
    shared_data.calculation_ready = false;
    shared_data.done = false;

    // Initialize mutex and condition variables
    pthread_mutex_init(&shared_data.mutex, NULL);
    pthread_cond_init(&shared_data.input_cond, NULL);
    pthread_cond_init(&shared_data.calc_cond, NULL);
    pthread_cond_init(&shared_data.write_cond, NULL);

    // Create threads
    pthread_create(&input_tid, NULL, input_thread, (void *)&shared_data);
    pthread_create(&calculator_tid, NULL, calculator_thread, (void *)&shared_data);
    pthread_create(&writer_tid, NULL, writer_thread, (void *)&shared_data);

    // Wait for threads to finish
    pthread_join(input_tid, NULL);
    pthread_join(calculator_tid, NULL);
    pthread_join(writer_tid, NULL);

    // Clean up
    pthread_mutex_destroy(&shared_data.mutex);
    pthread_cond_destroy(&shared_data.input_cond);
    pthread_cond_destroy(&shared_data.calc_cond);
    pthread_cond_destroy(&shared_data.write_cond);

    return 0;
}

void *input_thread(void *arg)
{
    SharedData *data = (SharedData *)arg;

    while (1)
    {
        printf("Enter calculation (number operation number, e.g., 5 * 3) or 'q' to quit: ");

        // Lock mutex before accessing shared data
        pthread_mutex_lock(&data->mutex);

        char input[100];
        fgets(input, sizeof(input), stdin);

        // Check for quit command
        if (input[0] == 'q' || input[0] == 'Q')
        {
            data->done = true;
            pthread_cond_signal(&data->input_cond); // Signal calculator to check done flag
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        // Parse input
        if (sscanf(input, "%lf %c %lf", &data->num1, &data->operation, &data->num2) == 3)
        {
            // Valid input
            data->input_ready = true;
            pthread_cond_signal(&data->input_cond); // Signal calculator thread
        }
        else
        {
            printf("Invalid input. Try again.\n");
            pthread_mutex_unlock(&data->mutex);
            continue;
        }

        // Wait until this input is processed
        while (data->input_ready)
        {
            pthread_cond_wait(&data->write_cond, &data->mutex);
        }

        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}

void *calculator_thread(void *arg)
{
    SharedData *data = (SharedData *)arg;

    while (1)
    {
        // Lock mutex before accessing shared data
        pthread_mutex_lock(&data->mutex);

        // Wait for input to be ready
        while (!data->input_ready && !data->done)
        {
            pthread_cond_wait(&data->input_cond, &data->mutex);
        }

       
        if (data->done)
        {
            pthread_cond_signal(&data->calc_cond); // Signal writer to check done flag
            pthread_mutex_unlock(&data->mutex);
            break;
        }

       
        switch (data->operation)
        {
        case '+':
            data->result = data->num1 + data->num2;
            break;
        case '-':
            data->result = data->num1 - data->num2;
            break;
        case '*':
            data->result = data->num1 * data->num2;
            break;
        case '/':
            if (data->num2 == 0)
            {
                printf("Error: Division by zero\n");
                data->input_ready = false;
                pthread_cond_signal(&data->write_cond);
                pthread_mutex_unlock(&data->mutex);
                continue;
            }
            data->result = data->num1 / data->num2;
            break;
        default:
            printf("Error: Unsupported operation\n");
            data->input_ready = false;
            pthread_cond_signal(&data->write_cond);
            pthread_mutex_unlock(&data->mutex);
            continue;
        }

       
        data->calculation_ready = true;
        data->input_ready = false;

        // Signal writer thread
        pthread_cond_signal(&data->calc_cond);
        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}

void *writer_thread(void *arg)
{
    SharedData *data = (SharedData *)arg;
    FILE *file = fopen("results.txt", "a");

    if (file == NULL)
    {
        perror("Error opening results.txt");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Lock mutex before accessing shared data
        pthread_mutex_lock(&data->mutex);

        // Wait for calculation to be ready
        while (!data->calculation_ready && !data->done)
        {
            pthread_cond_wait(&data->calc_cond, &data->mutex);
        }

        // Check if we should exit
        if (data->done && !data->calculation_ready)
        {
            pthread_mutex_unlock(&data->mutex);
            break;
        }

        // Write to file
        fprintf(file, "%.2f %c %.2f = %.2f\n",
                data->num1, data->operation, data->num2, data->result);
        fflush(file);

        printf("Result written to file: %.2f %c %.2f = %.2f\n",
               data->num1, data->operation, data->num2, data->result);

        // Reset flags
        data->calculation_ready = false;

        // Signal input thread that we're done with this calculation
        pthread_cond_signal(&data->write_cond);
        pthread_mutex_unlock(&data->mutex);
    }

    fclose(file);
    return NULL;
}
