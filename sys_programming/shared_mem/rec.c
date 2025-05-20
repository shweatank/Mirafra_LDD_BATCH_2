#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/shm.h>
int main()
{
		int d=shmget(123,1024,0644);
		void *ch;
		ch=shmat(d,NULL,0);
		printf("%s\n",(char *)ch);
		return 0;
}
