#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigusrl(int sig){
    printf("Received SIGUSR1! signal no: %d\n",sig);
}

int main(){
    signal(SIGQUIT,handle_sigusrl);
    printf("Receiver PID %d\n",getpid());


    while (1)
    {
    pause();
    }
    
}