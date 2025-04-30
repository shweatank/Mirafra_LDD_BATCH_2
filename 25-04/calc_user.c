#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<fcntl.h>

#define WR_VALUE _IOW(100,0,struct cal_buf *)
#define RD_VALUE _IOR(100,1,int *)

struct cal_buf
{
	int num1;
	int num2;
	char operator;
	int result;
};
int main()
{
struct cal_buf cal;

int fd,result;
fd=open("/dev/ioctl_calc",O_RDWR);

printf("enter values:");
scanf("%d %c %d",&cal.num1,&cal.operator,&cal.num2);

ioctl(fd,WR_VALUE,&cal);

ioctl(fd,RD_VALUE,&result);

printf("the result is :%d\n",result);

close(fd);
}
