#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd;
	char input[100];
	char output[100];
	fd=fopen("/dev/reverse",O_RDWR);

	printf("enter string:");
	fgets(input,sizeof(input),stdin);

	write(fd,intput,strlen(input));
	read(fd,output,sizeof(output));
	printf("the string is %s\n",output);
}
