//the Threads are having equal scheduled priorities

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
void *thr_func(void *args)
{
    int id = *(int *)args;
    printf("The thread %d is running\n", id);
}
int main()
{
    pthread_t p1, p2, p3;
    pthread_attr_t attr;
    int id1=1,id2=2,id3=3;
    struct sched_param param;
    
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    
    param.sched_priority = 50;
    pthread_attr_setschedparam(&attr, &param);
    
    pthread_create(&p1, &attr, thr_func, &id1);
    pthread_create(&p2, &attr, thr_func, &id2);
    pthread_create(&p3, &attr, thr_func, &id3);
    
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    
    pthread_attr_destroy(&attr);
}
