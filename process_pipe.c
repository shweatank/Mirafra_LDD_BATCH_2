#if 0
#include<stdio.h>
int main()
{
		while(1)
		{
				printf("Hi\n");
				sleep(2);
		}
		return 0;
}
#endif

#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#if 0
int main()
{
		pid_t p=fork();
		if(p<0)
		{
				perror("fork failed");
				return 1;
		}
		else if(p==0)
		{
				printf("child process: %d\n",getpid());
				execlp("ls","ls","-l",(char *)NULL);
				//If execlp fails
				perror("execlp failed");
				exit(EXIT_FAILURE);
		}
		else
		{
				printf("parent process: %d\n",getpid());
				wait(NULL);      //wait for child to finish
				printf("child finished... parent exiting\n");
		}
		return 0;
}
#endif

#if 1
#include<sys/stat.h>
#include<fcntl.h>
char buf[2];
int main()
{
		int fd[2];          //name can be anything like: int kavya[2]; 
		pipe(fd);
		pid_t p=fork();
		if(p<0)
		{
				perror("fork failed");
				return 1;
		}
		else if(p==0)
		{
				printf("child process: %d\n",getpid());
				//execlp("ls","ls","-l",(char *)NULL);
				//If execlp fails
				//perror("execlp failed");
				//exit(EXIT_FAILURE);
				close(fd[1]);
				read(fd[0],buf,sizeof(buf));
				int d=(buf[0]-'0') + (buf[1]-'0');
				printf("add res is %d\n",d);
				close(fd[0]);          //optional
		}
		else
		{
				close(fd[0]);
				int f=open("input.txt",O_RDONLY);
				//int buf[2];
				read(f,buf,sizeof(buf));
				write(fd[1],buf,sizeof(buf));
				printf("parent process: %d\n",getpid());
				close(fd[1]);     //optional
				wait(NULL);      //wait for child to finish
				//printf("child finished... parent exiting\n");
		}
		return 0;
}
#endif

#if 0
void parentFunction()
{
		printf("parent pid %d: Running parentFunction\n",getpid());
		for(int i=0;i<5;i++)
		{
				printf("Parent is working...%d\n",i+1);
				sleep(1);
		}
		printf("Parent finished work\n");
}
int main()
{
		pid_t p=fork();
		if(p<0)
		{
				perror("fork failed");
				exit(EXIT_FAILURE);
		}
		else if(p==0)
		{
				//In child
				printf("child pid %d: executing echo\n",getpid());
				execlp("echo","echo","child says hello from exec!",(char *)NULL);
				//If exec fails
				perror("execlp failed");
				exit(EXIT_FAILURE);
		}
		else
		{
				//in parent process
				parentFunction();
				//wait for child to finish
				wait(NULL);
				printf("Parent: child process has finished\n");
		}
		return 0;
}
#endif
