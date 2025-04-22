#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid = fork(); // create a new process
	if(pid < 0)
	{
	//printf("pid is less than 0\n");
	//fork failed
	perror("fork");
	exit(EXIT_FAILURE);
	}
	else if(pid ==0)
	{
	//printf("Pid value is equal to 0\n");

	//child process
	printf("Child process (PID: %d) is running 'ls -l'\n", getpid());
	execlp("ls", "ls", "-l", (char *)NULL);
	//printf("After execp fun\n");
	// if exec fails
	perror("execlp");
	exit(EXIT_FAILURE);
	}
	else
	{
	//parent process
	printf("Parent process (PID: %d) is waiting for child...\n", getpid());

	wait(NULL) ;// wait for child to finish
	//printf("after wait\n");

	printf("Child process finished. parent exiting.\n");
	}
}
