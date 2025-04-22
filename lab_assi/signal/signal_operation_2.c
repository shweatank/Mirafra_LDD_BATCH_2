#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
int a=50;
void handler_sigusr1(int sig){
	int c=a+sig;
	printf("recived signal number is no=%d\n",sig);	
	printf("recived addition is no=%d\n",c);
	printf("recived substraction is no=%d\n",a-sig);
	printf("recived multiplication is no=%d\n",a*sig);
}

int main(){
	signal(SIGUSR1,handler_sigusr1);
	printf("reciver pid=%d\n",getpid());
	
	while(1){
		pause();
	}
	return 0;
}
