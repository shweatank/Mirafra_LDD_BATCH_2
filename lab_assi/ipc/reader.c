#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/shm.h>
#include<sys/ipc.h>

#define SHM_SIZE 1024

int main(){
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

	printf("data read from memory=%s\n",str);

	shmdt(str);  //detach
	
	shmctl(shmid,IPC_RMID,NULL);  //destory shared memory

	return 0;

}






