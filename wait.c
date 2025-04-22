/*Write a c program to create a child process and make the parent to terminate after the child*/
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
main()//3 file descriptors(0,1,2---stdout,stdin,stderr)available(open)
{
	int ret;
	ret=fork();
	if(ret==-1)//failure
	{
		printf("error in creating the process\r\n");
		exit(0);
	}
	if(ret)//exclusive parent process
	{
		int childPID;
		int status=-2;
		printf("Before wait: \r\n");
		childPID=wait(&status);
		printf("After  wait: PID of the terminated child=%d \r\n",childPID);
	}
	else
	{//exclusively child1 process
		printf("I am in child1 process PID=%d PPID=%d\r\n",getpid(),getppid());
		sleep(10);
		printf("child with PID=%d is exiting\r\n",getpid());
	}

}
