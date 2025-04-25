#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/ioctl.h>
#define WR_VALUE _IOW(100,0,struct cal_buf *)
#define RD_VALUE _IOR(100,1,int *)
struct cal_buf
{
	int num1;
	int num2;
	char operation;
	int result;
};
int main()
{
	int fd,result;
	struct cal_buf cal;
	fd=open("/dev/calc",O_RDWR);
	printf("enter values:");
	scanf("%d %c %d",&cal.num1,&cal.operation,&cal.num2);
	ioctl(fd,WR_VALUE,&cal);
	ioctl(fd,RD_VALUE,&result);
	printf("result :%d\n",result);
	close(fd);
}
