#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
	int fd;
	char input[100];
	char output[100];

	fd=open("/dev/string_rev",O_RDWR);
	if(fd<0)
	{
		printf("error");
		return 0;
	}

	printf("enter string:");
	fgets(input,sizeof(input),stdin);

	write(fd,input,strlen(input));

	read(fd,output,sizeof(output));

	printf("reverse string:%s\n",output);
	close(fd);
}
