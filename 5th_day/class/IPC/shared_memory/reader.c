#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_SIZE 1024 //shared memory size

int main()
{
    key_t key = ftok("shmfile", 65); // Generaye a unique key
    int shmid = shmget(key, SHM_SIZE, 0666); //get the shared memory

    if(shmid == -1)
    {
        perror("shmget failed");
        exit(1);
    }
    char *str = (char *) shmat(shmid, NULL, 0); //attach
    if(str == (char *) -1)
    {
        perror("shmat failed");
        exit(1);
    }
    printf("Data read from memory : %s\n", str);
    shmdt(str); //detach
    shmctl(shmid, IPC_RMID, NULL); //destroy shared memory
    return 0;
}