#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig){
    printf("\nCaught signal %d (SIGNAL). Exiting gracefully\n",sig);
    exit(0);
}

int main(){
    signal(SIGINT,handle_sigint);

    while (1)
    {
        printf("Running .. press ctrl+c to stop\n");
        sleep(1);
    }
    
}