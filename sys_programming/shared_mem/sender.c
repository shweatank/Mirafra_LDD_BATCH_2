#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<sys/shm.h>
int main()
{
		char buf[20]="kavya";
		int d=shmget(123,1024,IPC_CREAT | 0644);
		void *str;
		str=shmat(d,NULL,0);
        strcpy(str,buf);
		return 0;
}
