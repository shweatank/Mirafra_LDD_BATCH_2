#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<wait.h>
#include <fcntl.h>
#include<string.h>

void *function();
void *operation();
void *file_save();
int no1,no2,output;
char ch[2];

pthread_mutex_t mutex1=PTHREAD_MUTEX_INITIALIZER;

int main(){
	pthread_t p1,p2,p3;

	pthread_create(&p1,NULL,&function,NULL);
	pthread_join(p1,NULL);

	pthread_create(&p2,NULL,&operation,NULL);
	pthread_join(p2,NULL);
	
	pthread_create(&p3,NULL,&file_save,NULL);
	pthread_join(p3,NULL);
	
	return 0;
}

void *function(){
	pthread_mutex_lock(&mutex1);

	printf("enter the first no\n");
	scanf("%d",&no1);
	printf("enter the secound no\n");
	scanf("%d",&no2);
	printf("enter the symbol no\n");
	scanf("%s",ch);
	
	pthread_mutex_unlock(&mutex1);
}

void *operation(){
	pthread_mutex_lock(&mutex1);
	if(strcmp(ch,"+")==0){
		output=no1+no2;
	}
	 else if(strcmp(ch,"-")==0){
		output=no1-no2;
	 }
	else if(strcmp(ch,"*")==0){
		output=no1*no2;
	}
	else if(strcmp(ch,"/")==0){
		output=no1/no2;
	}
	else
	printf("invalid operation\n");
	
	pthread_mutex_unlock(&mutex1);
}

void *file_save(){
		pthread_mutex_lock(&mutex1);
		FILE *fp;
		fp=fopen("result.txt","a");		
		if(fp==NULL){
			perror("erroe");
		        pthread_mutex_unlock(&mutex1);
			return NULL;
		}
		fprintf(fp,"output=%d\n",output);
		printf("output=%d\n",output);
		fclose(fp);

		pthread_mutex_unlock(&mutex1);
	}













