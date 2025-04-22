/*Write a c program to create 2 processes with common parent*/
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
main()//3 file descriptors(0,1,2---stdout,stdin,stderr)available(open)
{
	int child1PID;
	child1PID=fork();
	if(child1PID==-1)//failure
	{
		printf("error in creating the process\r\n");
		exit(0);
	}
	if(child1PID)//exclusive parent process
	{
		int child2PID,status=-2;
		child2PID=fork();
		if(child2PID)
		{//exclusive for the parent
			printf("I am in parent process PID=%d PPID=%d\r\n",getpid(),getppid());
			waitpid(child1PID,&status,0);//parent is waiting for child1 to terminate
			//waitpid(-1,&status,0);//parent is wait for one of the child to terminate(same like wait system call)
			printf(" parent process is getting terminated\r\n");
		}
		else
		{
			sleep(4);

			printf("I am in child2 process PID=%d getting terminated\r\n",getpid());
		}
	}
	else
	{//exclusively child1 process

		printf("In child1 process\r\n");
		sleep(10);
		printf("I am in child1 process PID=%d getting exited\r\n",getpid());
	}

}
