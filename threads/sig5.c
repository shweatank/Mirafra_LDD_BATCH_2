#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int a = 10, b = 3;

void handle_sigusr1(int sig)
{
    printf("Received SIGUSR1: Signal number: %d\n", sig);
    int add = a + b;
    printf("Sum of a + b = %d\n", add);
}

void handle_sigusr2(int sig)
{
    printf("Received SIGUSR2: Signal number: %d\n", sig);
    int sub = a - b;
    printf("Subtraction of a - b = %d\n", sub);
}

int main()
{
    printf("Receiver PID: %d\n", getpid());

    
    signal(SIGUSR1, handle_sigusr1);
    

    signal(SIGUSR1, handle_sigusr1);

    signal(SIGUSR2, handle_sigusr2);

    while (1)
    {
        pause(); // Wait for signal
    }

    return 0;
}

