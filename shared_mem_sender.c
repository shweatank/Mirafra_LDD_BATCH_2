#include<stdio.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<fcntl.h>
#include<string.h>
int main()
{
		void *mem;
		char buf[20];
		int id;
		id=shmget(123,1024,0644|IPC_CREAT);
		printf("id : %d\n",id);
		mem=shmat(id,NULL,0);           //0 means read / write access
		printf("Enter data");
		//read(0,buf,20);      //reading from standard input
		strcpy(buf,"kavya");
		printf("%s\n",buf);
		strcpy(mem,buf);
		shmdt(mem);
		return 0;
}
