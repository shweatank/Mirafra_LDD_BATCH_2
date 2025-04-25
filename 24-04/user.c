#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
int main()
{
	int fd;
	char input[100];
	char output[100];
	fd=open("/dev/char_square",O_RDWR);

	printf("enter a value:");
	fgets(input,sizeof(input),stdin);

	write(fd,input,strlen(input));

	read(fd,output,sizeof(output));

	printf("from kernel :%s\n",output);

	close(fd);
}
