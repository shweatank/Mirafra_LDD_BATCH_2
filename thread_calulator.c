#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
#include<pthread.h>

typedef struct 
{
    double num1;
    double num2;
    char operation;
    double result;
    int input_ready;
    int calc_done;
} SharedData;

SharedData data = {0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_input = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_calc = PTHREAD_COND_INITIALIZER;
void* input_thread(void* arg) 
{
    pthread_mutex_lock(&mutex);

    printf("Enter num1:");
    scanf("%lf", &data.num1);

    printf("Enter num2:");
    scanf("%lf", &data.num2);

    printf("Enter operation: ");
    scanf(" %c", &data.operation);

    data.input_ready = 1;
    pthread_cond_signal(&cond_input);

    pthread_mutex_unlock(&mutex);
    return NULL;
}
void* calculation_thread(void* arg)
{
    pthread_mutex_lock(&mutex);

   while (!data.input_ready) {
        pthread_cond_wait(&cond_input, &mutex);
 }

    switch (data.operation) 
    {
        case '+': data.result = data.num1 + data.num2; break;
        case '-': data.result = data.num1 - data.num2; break;
        case '*': data.result = data.num1 * data.num2; break;
        case '/':
            if (data.num2 != 0)
                data.result = data.num1 / data.num2;
            else
                data.result = 0.0 / 0.0; // NaN
            break;
        default:
            printf("Invalid operation\n");
            data.result = 0;
    }
    data.calc_done = 1;
    pthread_cond_signal(&cond_calc);
    pthread_mutex_unlock(&mutex);

    return NULL;
}
void* logging_thread(void* arg)
{
    pthread_mutex_lock(&mutex);

     char buffer[20];
    int len = snprintf(buffer, sizeof(buffer), "%lf\n", data.result);
   while (!data.calc_done) 
   {
        pthread_cond_wait(&cond_calc, &mutex);
    }
    int fp=open("file1.txt",O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fp==-1)
    {
     printf("%d\n",fp);
       printf("fail to open file\n");
       return 0;
   }
   else
   {
       write(fp,buffer,len);
       printf("%lf\n",data.result);
        close(fp);
        printf("Result written to results.txt\n");
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}
int main()
{
    pthread_t inpu_thread, calc_thread, log_thread;
    
    pthread_create(&inpu_thread, NULL, input_thread, NULL);
    pthread_create(&calc_thread, NULL, calculation_thread, NULL);
    pthread_create(&log_thread, NULL, logging_thread, NULL);

    pthread_join(inpu_thread, NULL);
    pthread_join(calc_thread, NULL);
    pthread_join(log_thread, NULL);
    
    return 0;
}
