
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
void *function();
pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER;
int count=0;

int main(){
	int z=100;
	pthread_t p[z];
	for(int i=0;i<z;i++){
	pthread_create(&p[i],NULL,&function,NULL);
	}
	
	for(int i=0;i<z;i++){
	pthread_join(p[i],NULL);
	}

	printf("final value of counter=%d",count);
}

void *function(){
	printf("thread number=%ld\n",pthread_self());
	printf("count value=%d\n",count);
	pthread_mutex_lock(&mutex1);
	count++;
	pthread_mutex_unlock(&mutex1);
}



