#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define SHM_SIZE 1024
int main(){
key_t key=ftok("shmfile",65);
int id=shmget(key,SHM_SIZE,0666|IPC_CREAT);
if(id==-1){
perror("shmget:");
return 0;
}
char *ptr=(char*)shmat(id,NULL,0);
if(ptr==(char *)-1){
perror("shmat:");
return 0;
}
printf("write,the data\n");
printf("%s\n",ptr);
shmdt(ptr);
}
