#include<stdio.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
	pid_t pid;
	pid =fork();
	if(pid<0)
	{
		printf("error\n");
	}
	else if(pid==0)
	{
		printf("child process is %d\n",getpid());
		exit(0);
	}
	else
	{
		printf("parent process id is %d\n",getpid());
		sleep(30);
	}
}
