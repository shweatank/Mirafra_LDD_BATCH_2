#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
int main()
{
	int shmid;
	key_t key=ftok("shmfile",60);
	shmid=shmget(key,1024,0666);
	char *ptr=(char *)shmat(shmid,NULL,0);
	printf("the data received is %s\n",ptr);
	shmdt(ptr);
	shmctl(shmid,IPC_RMID,NULL);
}
