#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
void *function();
pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER;
int count=0;

int main(){
	int r1,r2;
	pthread_t p1,p2;

	if(r1=pthread_create(&p1,NULL,&function,NULL)){
		printf("failed=%d\n",r1);
	}
	if(r2=pthread_create(&p2,NULL,&function,NULL)){
		printf("failed=%d\n",r2);
	}
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);

	exit(0);
}
void *function(){
	pthread_mutex_lock(&mutex1);
	count++;
	printf("count value=%d\n",count);
	pthread_mutex_unlock(&mutex1);

}

