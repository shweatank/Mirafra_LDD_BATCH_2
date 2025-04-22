#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_SIZE 1024

int main(){
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);

    if (shmid == -1){
        perror("shmget failed");
        exit(1);
    }
    char *str = (char*) shmat(shmid,NULL,0);
    if (str == (char*)-1){
        perror("shmat failed");
        exit(1);
    }
    printf("write Data: ");
    fgets(str, SHM_SIZE, stdin);

    printf("data written in memory: %s\n",str);

    shmdt(str);

    return 0;
}