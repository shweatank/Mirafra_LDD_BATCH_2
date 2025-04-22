#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>


void handle_sigusr1(int sig){
	printf("recived SIGUSR1  signal number: %d\n",sig);
}

int main(){
	signal(SIGUSR1,handle_sigusr1);
	printf("reciver PID: %d\n",getpid());

	while(1){
		pause();   //wait for a signal
			   
	}
}
