#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(){
    pid_t pid;
    printf("enter pid to send SIGUSR1: ");
    scanf("%d",&pid);

    kill(pid,SIGQUIT);
    printf("sent SIGUSR1 to PID %d\n", pid);

    return 0;

}