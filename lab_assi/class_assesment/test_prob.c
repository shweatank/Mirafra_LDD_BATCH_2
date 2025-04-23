#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>

#define SHM_SIZE 1024
int main(){
 
	pid_t pid=fork();

	if(pid<0)
		perror("error\n");
	else if(pid==0){
	
	key_t key=ftok("shmfile",65);  //unique key
	int shmid=shmget(key,SHM_SIZE,0666 | IPC_CREAT);  //shared memory
	
	if(shmid==-1){
		perror("failed\n");
		exit(1);
	}
	char *str=(char*)shmat(shmid,NULL,0);  //attached shared memory
	if(str==(char *)-1){
		perror("shmat failed\n");
		exit(1);
	}
		int no1,no2;
		printf("enter no1 and no2\n");
		scanf("%d%d",&no1,&no2);
		
	}
	else
	{	wait(NULL);
		printf("in parent process\n");
		*str=no1+no2;
		printf("addition=%d\n",*str);
	}
	return 0;

}

