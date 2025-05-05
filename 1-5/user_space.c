#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define size 100

int main()
{
	char msg[]="vishnu";
	char buf[size]={0};
	int fd;
	fd=open("/dev/mydevice",O_RDWR);
	if(fd==-1)
	{
		printf("failed to open the device:");
		return 0;
	}

	if(write(fd,msg,strlen(msg)+1)<0)
	{
		printf("the message sending is failed..\n");
		return 0;
	}
	printf("data received is %s\n",msg);

	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("the message received is failed..\n");
		return 0;
	}
	printf("data is %s\n",buf);

	close(fd);
	return 0;
}
