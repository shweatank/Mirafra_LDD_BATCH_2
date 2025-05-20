#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
int main()
{
		int fd[2];
		char buf[6];
		pid_t p=fork();
		if(p>0)        //parent process
		{
				close(fd[0]);             //close read process
				write(fd[1],"kavya",5);
				wait(NULL);
		}
		else if(p==0)
		{
				close(fd[1]);        //close write end
				read(buf,fd[0],5);
				printf("%s\n",buf);
				exit(0);
		}
		else
		{
				perror("failed");
				return 1;
		}
		return 0;
}
