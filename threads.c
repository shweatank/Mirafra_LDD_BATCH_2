#include<pthread.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
pthread_t t1,t2,t3;
int a,b,res,signal1=0,signal2=0;
char ch;
pthread_mutex_t m1=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1=PTHREAD_COND_INITIALIZER;
void *Input(void *ptr){
	pthread_mutex_lock(&m1);
printf("enter the input a ,ch and b:");
scanf("%d",&a);
while(getchar()!='\n');
ch =getchar();
while(getchar()!='\n');
scanf("%d",&b);
printf("a=%d %d b=%d\n",a,ch,b);
signal1=1;
pthread_cond_signal(&cond);
pthread_mutex_unlock(&m1);
}

void *calculation(void *ptr){
	pthread_mutex_lock(&m1);
	while(!signal1){
	pthread_cond_wait(&cond,&m1);
	}
	printf("calculation\n");
	signal2=1;
	switch(ch){
		case '+':res=a+b;
			 break;
		case '-':res=a-b;
			 break;
	}
	pthread_cond_signal(&cond1);
printf("the result is:%d\n",res);
	pthread_mutex_unlock(&m1);
}

void *file(void *ptr){
	pthread_mutex_lock(&m1);
	while(!signal2){
	pthread_cond_wait(&cond1,&m1);
	}
	printf("file\n");
	int fd;
	fd=open("result.txt", O_WRONLY|O_TRUNC );
	char buffer[10];
	memset(buffer,'\0',sizeof(buffer));
	sprintf(buffer,"%d %c %d = %d",a,ch,b,res);
	write(fd,buffer,strlen(buffer));
	close(fd);
	pthread_mutex_unlock(&m1);
}





int main(){
pthread_create(&t1, NULL, Input,NULL);
pthread_create(&t2, NULL, calculation,NULL);
pthread_create(&t3, NULL, file,NULL);

pthread_join(t1,NULL);
pthread_join(t2,NULL);
pthread_join(t3,NULL);

}
