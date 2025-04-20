
/*Multiprocessing Factorial Calculator
Problem:
Given an array of N numbers, fork N processes.
Each process calculates the factorial of its number and writes the result into a shared memory array.
The parent waits and prints the full result array.*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

long long fact(int num)
{
    if(num==1 || num==0)
    {
        return 1;
    }
   long long result = 1;
    for (int i = 2; i <= num; i++)
        result *= i;
    return result;
}
int main() 
{
    int N;
    printf("Enter the N value:");
    scanf("%d",&N);
    int arr[N];
    printf("Enter the array elemnts:");
    for(int i=0;i<N;i++)
    {
        scanf("%d",&arr[i]);
    }
    int shm_id=shmget(IPC_PRIVATE,N*sizeof(long long),IPC_CREAT | 0666);
    if(shm_id<0)
    {
        printf("failed to allocate the shared memory\n");
        return 0;
    }
    long long *shared_mem=(long long *)shmat(shm_id,NULL,0);
    if(shared_mem==(void *)-1)
    {
        printf("Failed to attatch\n");
      exit(0);
    }
    for(int i=0;i<N;i++)
    {
        pid_t pid=fork();
        if(pid<0)
        {
            printf("Failed to creat process\n");
           return 0;
        }
        else if (pid == 0) 
        {
            shared_mem[i]=fact(arr[i]);
            exit(0);
        }
    }
     for (int i = 0; i < N; i++) {
        wait(NULL);
    }
    for(int i=0;i<N;i++)
    {
       printf("factorial of %d is %lld\n", arr[i], shared_mem[i]);
       
    }
    shmdt(shared_mem);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
    
 
