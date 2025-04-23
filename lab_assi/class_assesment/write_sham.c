#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_SIZE 1024
int main(){
	pid_t pid=fork();
	
	if(pid<0){
		perror("error\n");
		exit(1);
	}
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
	printf("in child=%d\n",getpid());
	printf("enter the input\n");
	scanf("%d %d",&no1,&no2);
	printf("two number are %d %d\n",no1,no2); 

	str[0]=no1;
	str[1]=no2;
	
	shmdt(str);  //detached from shared memory
	}
	
	else{
	wait(NULL);
	
	printf("in parent=%d\n",getpid());
	key_t key=ftok("shmfile",65);
	int shmid=shmget(key,SHM_SIZE, 0666); //get shared memory

	if(shmid==-1){
		perror("shmget failed");
		exit(1);
	}

	char *str=(char*)shmat(shmid,NULL,0); //attached
	if(str==(char*)-1){
		perror("shmat failed\n");
		exit(1);
	}

	printf("data read from memory=%d\n",str[0]+str[1]);

	shmdt(str);  //detach
	
	shmctl(shmid,IPC_RMID,NULL);  //destory shared memory
	
	}
	
	return 0;

}






