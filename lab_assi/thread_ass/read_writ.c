#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER; //mutex
pthread_mutex_t mutex_rw=PTHREAD_MUTEX_INITIALIZER;  //read_write 
pthread_cond_t cond =PTHREAD_COND_INITIALIZER;

void* read_thread(void* );
void* write_thread(void* );
int read_count=0;
int write_waiting=0;


int main(){
	int size=3;
	pthread_t read_thread[size],write_thread[size];
	
	for(long i=0;i<size;i++){
	pthread_create(&read_thread[i],NULL,read_thread,(void*)i);
	}

	for(long i=0;i<size;i++){
	pthread_create(&write_thread[i],NULL,write_thread,(void*)i);
	}
	
	for(int i=0;i<size;i++){
	pthread_join(read_thread[i],NULL);
	}
	
	for(int i=0;i<size;i++){
	pthread_join(write_thread[i],NULL);
	}


	return 0;
}

void* read_thread(void* a){
	long read_id=(long)a;
	pthread_mutex_lock(&mutex1);

	while(write_waiting>0){
		pthread_cond_wait(&cond, &mutex1);
	}
	
	read_count++;
	if(read_count==1){
		pthread_mutex_lock(&mutex_rw);
	}

	pthread_mutex_unlock(&mutex1);
	printf("reader id=%ld\n",read_id);
	sleep(1);

	pthread_mutex_lock(&mutex1);
	read_count--;
	if(read_count==0){
		pthread_mutex_unlock(&mutex_rw);
	}
	pthread_mutex_unlock(&mutex1);

	return NULL;
}

void* write_thread(void* b){

	long write_id=(long)b;
	pthread_mutex_lock(&mutex1);
	
	write_waiting++;
	while(read_count > 0 || write_waiting > 1){
		pthread_cond_wait(&cond,&mutex1);
	}
	write_waiting--;
	pthread_mutex_unlock(&mutex1);

	pthread_mutex_lock(&mutex_rw);
	printf("write id=%ld\n",write_id);
	sleep(2);
	pthread_mutex_unlock(&mutex_rw);

	pthread_cond_broadcast(&cond);  //release all resources

	return NULL;
}



