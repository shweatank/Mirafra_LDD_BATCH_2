#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

void handler(int sig){
	printf("signal %d recived\n",sig);
}

int main(){
	signal(SIGINT,handler);

	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set,SIGINT);

	printf("blocking SIGINT for 5 secound i..\n");
	sigprocmask(SIG_BLOCK,&block_set,NULL);
	sleep(5);

	printf("unblocking SIGINT, TRy processing Cont+c now.\n");
	sigprocmask(SIG_UNBLOCK,&block_set,NULL);

	while(1){
		pause();
	}
}
