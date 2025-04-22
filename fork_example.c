#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
int a,b;
int main()
{
	FILE *fp;
	char buf[100];
	fp=fopen("v1.txt","r");
	fscanf(fp,"%d %d",&a,&b);
	fclose(fp);
	pid_t pid=fork();
	if(pid<0)
	{
		printf("error");
		exit(EXIT_FAILURE);
	}
	else if(pid==0)
	{
		
		printf("sum=%d\n",a+b);
		exit(EXIT_FAILURE);
	}
	else
	{
		wait(NULL);
		printf("child process is completed\n");
	}
}
