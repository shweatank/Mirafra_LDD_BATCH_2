#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>

#define KEYBOARD_IRQ 1
int main()
{
	char input[10];
	char output[100];
	FILE *fp;
	fp=open("/dev/keyboard",O_RDWR);
	int a,b;
	printf("enter a and b values:");
	scanf("%d %d",&a,&b);
	write(fp,input,sizeof(input));
	read(fd,output,sizeof(output));
	printf("result=%d\n",output);
	close(fp);
}
