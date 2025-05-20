#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
int a = 10;
int b = 20;
void sum(int sig){
    if (sig == 3)
    {
    int sum = a + b;
    printf("Sum is %d\n", sum);
    }
    printf("Received SIGUSR1! signal no: %d\n",sig);    
}

void diff(int sig){
    if (sig == 10)
    {
    int sub = a - b;
    printf("Sub is %d\n",sub);    
    }
    printf("Received SIGUSR1! signal no: %d\n",sig);    
}

int main(){
    signal(SIGQUIT,sum);
    signal(SIGUSR1,diff);
    printf("Receiver PID %d\n",getpid());


    while (1)
    {
    pause();
    }
    
}