#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<string.h>

int main()
{
		char buf[100];
		int fd=open("/dev/key_int_kernel", O_RDWR);
		printf("Enter character\n");
		fgets(buf,sizeof(buf),stdin);
		write(fd,buf,sizeof(buf));
		return 0;
}
