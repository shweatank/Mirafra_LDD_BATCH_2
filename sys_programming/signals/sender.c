#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
		//int d=getpid();
		pid_t p;
		printf("Enter pid\n");
		scanf("%d",&p);
		kill(p,SIGUSR1);
		printf("hello\n");
		return 0;
}
