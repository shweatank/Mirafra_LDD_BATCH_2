#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
int add(int a,int b){
	return a+b;
}
int main(){
	int a=0,b=0;
	char ch;
	int fd=open("result",O_RDONLY);
	char buffer[10];
	int i=0;
	do{
		read(fd,&buffer[i],1);
	}while(buffer[i++]!='\n');
	buffer[--i]='\0';
	//printf("%s\n",buffer);
	int j=0;
	while(buffer[j]!=',')
		a=a*10+(buffer[j++]-48);
	j++;
	while(buffer[j]!='\0'){
		b=b*10+(buffer[j++]-48);
	}
	close(fd);
	//printf("%d,%d",a,b);
	pid_t pid=fork();

	if(pid==0){

		int res=add(a,b);
		int fd1=open("result",O_WRONLY|O_APPEND);
		char buf[10];
		sprintf(buf,"%d +%d = %d\n",a,b,res);
		write(fd1,buf,10);
		printf("buf:%s\n",buf);
		close(fd1);
	}
	else{
		wait(NULL);
	}
}
