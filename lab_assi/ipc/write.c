#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_SIZE 1024
int main(){
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

	printf("writing data\n");
	fgets(str,SHM_SIZE,stdin);   //write data to shared memory
	
	printf("data written in memory=%s\n",str);

	shmdt(str);  //detached from shared memory
	
	return 0;

}






