#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

void parentfun()
{
	printf("Parent (PID: %d): Running parentFunction()\n", getpid());
	for(int i=0;i<5;i++) {
		printf("Parent is waiting...(%d)\n", i+1);
		sleep(1);
	}
	printf("Parent finished work. \n");
}

int main()
{
	pid_t pid = fork();

	if(pid<0)
	{
	perror("fork failed");
	exit(EXIT_FAILURE);
	}
	else if(pid ==0)
	{
		printf("Child (PID: %d): Executing 'echo' \n", getpid());
		execlp("echo", "echo", "Child says hello from exec",(char *)NULL);

		perror("execlp failed");
		exit(EXIT_FAILURE);
	}
	else{
		parentfun();
		wait(NULL);
		printf("Parent: Child process has finished. \n");
	}
	return 0;
}


