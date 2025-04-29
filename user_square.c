#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>

int main()
{
		int fd;
		char input[100];
		char output[100];

		fd=open("/dev/kernel_square", O_RDWR);
		if(fd<0)
		{
				perror("failed to open\n");
				return 1;
		}
		printf("Enter number\n");
		fgets(input,sizeof(input),stdin);

		write(fd,input,sizeof(input));
		read(fd,output,sizeof(output));

		printf("square is %s\n",output);
		return 0;
}
