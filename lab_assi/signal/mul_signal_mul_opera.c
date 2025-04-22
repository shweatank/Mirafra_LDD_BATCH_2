#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

int main(){
	pid_t pid1,pid2;
	printf("enter the pid of process\n");
	scanf("%d",&pid1);
	kill(pid1,SIGUSR1);
	printf("sent SEGUSR1 to pid=%d\n",pid1);

	printf("enter the pid of process\n");
	scanf("%d",&pid2);
	kill(pid2,SIGUSR2);
	printf("sent SEGUSR2 to pid=%d\n",pid2);

	/*printf("enter the pid of process\n");
	scanf("%d",&pid3);
	kill(pid3,SIGUSR3);
	printf("sent SEGUSR3 to pid=%d\n",pid3);
	*/

	return 0;

}
