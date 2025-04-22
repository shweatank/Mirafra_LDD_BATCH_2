#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
int a,b;

int main(){
	FILE *fp=fopen("result.txt","r");
	int buffer[20];
	int k=fread(buffer,1,sizeof(buffer),fp);
	buffer[k]='\0';
	printf("read data=%s",buffer);
	//a=atoi(buffer[0]);
	//b=atoi(buffer[2]);

	fclose(fp);
	sscanf(buffer, "%d %d", &a, &b);
 
	pid_t pid=fork();
	if(pid<0){
		perror("error\n");
		exit(1);
	}
	else if(pid==0){
		printf("in child process=%d\n",getpid());
		printf("addition=%d\n",a+b);
		printf("substraction=%d\n",a-b);

	}
	else{
		printf("in parent process=%d\n",getpid());
		wait(NULL);	
	}

	return 0;
}
