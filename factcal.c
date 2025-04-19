/*
Multiprocessing Factorial Calculator
Problem:
Given an array of N numbers, fork N processes.
Each process calculates the factorial of its number and writes the result into a shared memory array.
The parent waits and prints the full result array.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

long long factorial(int n) {
    if (n == 0 || n == 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    
     
    int n = 5;
    int arr[5];
    printf("Enter any 5 the array elemnts:");
     for(int i=0;i<n;i++)
     {
         scanf("%d",&arr[i]);
     }

    
    int shmid = shmget(IPC_PRIVATE, n * sizeof(long long), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("share memory faild \n");
        return 0;
    }

    
    long long* results = ( long long*) shmat(shmid, NULL, 0);
    if (results == (void*) -1) {
        perror("share memory faild to attach\n");
        return 0;
    }

    
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            
            results[i] = factorial(arr[i]);
            shmdt(results);  
            exit(0);
        }
    }

   
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

 
    printf("Factorials:\n");
    for (int i = 0; i < n; i++) {
        printf("%d! = %lld\n", arr[i], results[i]);
    }

  
    shmdt(results);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
