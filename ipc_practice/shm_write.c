#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>

int main()
{
	int shmid;
	key_t key=ftok("shmfile",60);
	shmid=shmget(key,1024,0666|IPC_CREAT);
	char *shmptr=(char *)shmat(shmid,NULL,0);
	printf("enter anything you like:");
	fgets(shmptr,1024,stdin);
	printf("the data enter is:%s\n",shmptr);
	shmdt(shmptr);
}
