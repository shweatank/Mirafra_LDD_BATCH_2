#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig){
printf("Handled Signal %d using sigaction\n",sig);
}

int main(){

	struct sigaction sa;
	sa.sa_handler = sig_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	
	sigaction(SIGINT, &sa, NULL);
	
	while(1){
	printf("waiting for SIGINT\n");
	sleep(2);
    return 0;
}
}
