#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<string.h>
#include<sys/wait.h>

int main()
{
	int fd[2];
	char *msg="hello,vishnu!!!";
	char buf[100];
	if(pipe(fd)==-1)
	{
		printf("creation failed");
	}
	pid_t pid=fork();
	if(pid<0)
	{
		printf("error");
		exit(0);
	}
	else if(pid==0)
	{
		close(fd[1]);
		read(fd[0],buf,sizeof(buf));
		printf("msg=%s",buf);
		close(fd[0]);
	}
	else
	{
	close(fd[0]);
write(fd[1],msg,strlen(msg)+1);
printf("message send to child\n");
wait(NULL);
close(fd[1]);
	}
}
