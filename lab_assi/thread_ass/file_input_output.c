#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void* incr_fun();
void* decr_fun();
int counter=0;
pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER;
int main(){
	pthread_t p1,p2;

	pthread_create(&p1,NULL,&incr_fun,NULL);
	pthread_create(&p2,NULL,&decr_fun,NULL);

	pthread_join(p1,NULL);
	pthread_join(p2,NULL);

	return 0;
}

void* incr_fun(){
	counter++;
	printf("counter=%d\n",counter);
	
}
void* decr_fun(){
	counter--;
	printf("counter=%d\n",counter);
}

