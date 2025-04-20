/*
 Multiprocessing Factorial Calculator
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>


unsigned long long factorial(int n) {
    unsigned long long res = 1;
    for (int i = 2; i <= n; i++)
        res *= i;
    return res;
}

int main() {
    int size_array;
    printf("Enter the size of array : ");
    scanf("%d",&size_array);
    
    int numbers[size_array];
    printf("Enter %d elements : ",size_array);
    for(int i = 0; i < size_array; i++){
        scanf("%d",&numbers[i]);
    }
    
    int n = sizeof(numbers) / sizeof(numbers[0]);

    int shmid = shmget(IPC_PRIVATE, n * sizeof(unsigned long long), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }

    unsigned long long *results = (unsigned long long *)shmat(shmid, NULL, 0);
    if (results == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {      
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            results[i] = factorial(numbers[i]);
            shmdt(results);  
            exit(0);         
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    for (int i = 0; i < n; i++) {
        printf("Factorial of %d = %llu\n", numbers[i], results[i]);
    }

    shmdt(results);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
