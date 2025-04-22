#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
int a=50;
void handler_sigusr1(int sig){
	printf("recived signal number=%d\n",sig);
	printf("addition=%d\n",a+sig);
}
void handler_sigusr2(int sig){
	printf("recived signal number=%d\n",sig);
	printf("substraction=%d\n",a-sig);
}

int main(){
	printf("recive pid=%d\n",getpid());
	signal(SIGUSR1,handler_sigusr1);
	
	signal(SIGUSR2,handler_sigusr2);

//	signal(SIGUSR3,handler_sigusr3);
//	printf("recive pid=%d\n",getpid());
	
	while(1){
		pause();
	}

	return 0;
}






