// read the data on parent process and do the opeartion (Addition) on child process using shared memory

#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

typedef struct {
    int num1;
    int num2;
    int result;
} shared_data;

int main()
{
    key_t key = ftok("shmfile", 65) ;//unique generate key
    if(key < 0)
    {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(key, sizeof(shared_data), 0666 | IPC_CREAT);
    if(shmid == -1)
    {
        perror("shmget");
        exit(1);
    }

    shared_data *data = (shared_data *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat");
        exit(1);
    }
     //parent process
     printf("Enter the num1 : ");
     scanf("%d", &data->num1);
     printf("Enter the num2 : ");
     scanf("%d", &data->num2);
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if(pid > 0)
    {
       

        wait(NULL); //wait for child process

        printf("Child process added to the results %d + %d = %d\n", data->num1, data->num2, data->result);

        //detach
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        
    }
    else{
        //child process
       

        data->result = data->num1 + data->num2;

        //detach
        shmdt(data);
        exit(0);
    }
    return 0;
}