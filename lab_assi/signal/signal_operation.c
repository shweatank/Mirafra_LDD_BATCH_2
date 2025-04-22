#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

int main(){
	pid_t pid;
	printf("enter the pid of process\n");
	scanf("%d",&pid);

	kill(pid,SIGUSR1);
	printf("sent SEGUSR1 to PID=%d\n",pid);
	


	return 0;
}

