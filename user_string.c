#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main()
{
		int fd;
		char input[100];
		char output[100];
		fd=open("/dev/kernel_string",O_RDWR);
		if(fd<0)
		{
				perror("fail open failed\n");
			   return 1;
		}	   
		printf("Enter string to reverse\n");
		fgets(input,sizeof(input),stdin);

		write(fd,input,strlen(input));
		read(fd,output,sizeof(output));

		printf("reversed string is %s\n",output);

		return 0;
}
