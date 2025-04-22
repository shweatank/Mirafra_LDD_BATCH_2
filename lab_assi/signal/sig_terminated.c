#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

void handle_sigint(int sig){
	sig=10;
	printf("\n%d\n",sig);
	printf("caught signal..terminated sucessfully\n");
	exit(0);
}

int main(){
	signal(SIGINT,handle_sigint);   //cont+c
	
	while(1){
		printf("hello world...\n");
		sleep(1);
	}

	return 0;
}
