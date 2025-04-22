#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
int main()
{
	pid_t pid=fork();
	if(pid<0)
	{
		printf("creation of process is failed\n");
		printf("HI:");
		exit(EXIT_FAILURE);
	}
	else if(pid==0)
	{
		printf("child process id is %d\n",getpid());
		printf("in child process ::hello all:\n");
		execlp("ls","ls","-l",(char *)NULL);
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("parent process id is %d\n",getpid());
		wait(NULL);
		printf("parent proces:::hi ,vishnu!!!\n");
		printf("successfull\n");
	}
}
