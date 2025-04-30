#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd;
	char input[100];
	char output[100];
	fd=open("/dev/keyboard",O_RDWR);
	printf("enter a nd b avlyes:");
	scanf("%d %d",&a,&b);
	write(fd,input,sizeof(input));
	read(fd,output,sizeof(output));
	printf("result=%d\n",output);
	close(fd);
}
