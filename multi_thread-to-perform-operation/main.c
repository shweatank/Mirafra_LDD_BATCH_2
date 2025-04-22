#include<stdio.h>
#include<pthread.h>
pthread_mutex_t lock;
int a,b,result;
pthread_cond_t cond_input,cond_result;
int input_ready=0;
int result_ready=0;
void* input(void *arg)
{
    pthread_mutex_lock(&lock);
    printf("enter two values:");
    scanf("%d %d",&a,&b);
    input_ready=1;
    pthread_cond_signal(&cond_input);
    pthread_mutex_unlock(&lock);
}
void* operation(void* arg)
{
    pthread_mutex_lock(&lock);
    while(!input_ready)
    {
        pthread_cond_wait(&cond_input,&lock);
    }
    result=a+b;
    printf("the result of %d + %d=%d", a,b,result);
    result_ready=1;
    pthread_cond_signal(&cond_result);
    pthread_mutex_unlock(&lock);
}
void* file(void* arg)
{
    pthread_mutex_lock(&lock);
    while(!result_ready)
    {
        pthread_cond_wait(&cond_result,&lock);
    }
    FILE *fp;
    fp=fopen("result.txt","w");
    fprintf(fp,"the result is",result);
    fclose(fp);
    pthread_mutex_unlock(&lock);
}
int main()
{
    pthread_t t1,t2,t3;
    pthread_create(&t1,NULL,&input,NULL);
    pthread_create(&t2,NULL,&operation,NULL);
    pthread_create(&t3,NULL,&file,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
}