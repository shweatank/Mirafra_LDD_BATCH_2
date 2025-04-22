#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigusr1(int sig)
{
	printf("Received SIGUSR1: Signal number: %d\n", sig);
}
int main()
{
	signal(SIGUSR1, handle_sigusr1);
	printf("Receiver PID: %d\n", getpid());

	while(1){
		pause(); //wait for signal
			
	}
}
