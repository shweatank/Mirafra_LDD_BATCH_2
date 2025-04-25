#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>

#define WR_VALUE _IOW(100,0,char *)
#define RD_VALUE _IOR(100,1,char *)

int main()
{
	char string[100];
	char result[100];

	int fd;
	fd=open("/dev/string_rev",O_RDWR);
	printf("enter string:");
	fgets(string,sizeof(string),stdin);
	ioctl(fd,WR_VALUE,string);
	ioctl(fd,RD_VALUE,result);
	printf("string is:%s\n",result);
	close(fd);
}
