#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>

void parentfunction()
{
	printf("parent process id is %d",getpid());
	for(int i=0;i<5;i++)
	{
		printf("parent is working....%d\n",i+1);
		sleep(1);
	}
	printf("parent finished the work\n");
}

int main()
{
	pid_t pid=fork();
	if(pid<0)
	{
		printf("error");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0)
	{
		printf("child process is working with id:%d", getpid());
		exit(EXIT_FAILURE);
	}
	else
	{
		parentfunction();
		wait(NULL);
		printf("completed\n");
	}
}
