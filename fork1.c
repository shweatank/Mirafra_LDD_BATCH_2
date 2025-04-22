#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
main()
{
	int ret;
	printf("I am in Main...PID=%d\n",getpid());
	ret=fork();
	printf("PID=%d\n",getpid());
}
