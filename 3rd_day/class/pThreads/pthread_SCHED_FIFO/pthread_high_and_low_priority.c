//the thread is scheduled to high and low priorities

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

void *low_func(void *args)
{
    int id = *(int *)args;
    printf("The Thread %d is running \n", id);
    return NULL;
}
void *high_func(void *args)
{
    int id = *(int *)args;
    printf("The Thread %d is running \n", id);
    return NULL;
}
int main()
{
    pthread_t p1, p2, p3;
    pthread_attr_t high_attr, low_attr;
    int id1=1,id2=2,id3=3;
    struct sched_param high_param, low_param;
    
    pthread_attr_init(&high_attr);
    pthread_attr_init(&low_attr);
    
    pthread_attr_setschedpolicy(&high_attr, SCHED_FIFO);
    pthread_attr_setschedpolicy(&low_attr, SCHED_RR);
    
    high_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    low_param.sched_priority = sched_get_priority_min(SCHED_RR);
    
    pthread_attr_setschedparam(&high_attr, &high_param);
    pthread_attr_setschedparam(&low_attr, &low_param);
    
    pthread_create(&p1, &low_attr, low_func, &id1);
    pthread_create(&p2, &high_attr, high_func, &id2);
    pthread_create(&p3, &low_attr, low_func, &id3);
    
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    
    pthread_attr_destroy(&high_attr);
    pthread_attr_destroy(&low_attr);
    
    
}
