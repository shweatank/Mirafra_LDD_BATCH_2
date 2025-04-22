#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
main()
{
	int ret;
	printf("I am in Main...PID=%d\n",getpid());
	ret=fork();
	if(ret==-1)
	{
		perror("Create");
		exit(0);
	}
	else if(ret>0)
	{//exclusive paranet process
		printf("Iam in parent process PID=%d\n",getpid());
	}
	else
	{
		printf("Iam child process PID=%d\n",getpid());
	}
}
