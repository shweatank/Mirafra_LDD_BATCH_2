#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(){
	pid_t pid=fork();

	if(pid<0){
		perror("error");
		exit(1);
	}
	else if(pid==0){
		printf("child pid=%d\n",getpid());
	//	execlp("ls","ls","-l",(char *)NULL);
		execlp("/usr/bin/pwd","pwd",NULL);

		perror("error");
		exit(1);
	}
	else
	{
		printf("pareent pid=%d\n",getpid());
		wait(NULL);
		printf("child finished the task\n");
	}

	return 0;
}
