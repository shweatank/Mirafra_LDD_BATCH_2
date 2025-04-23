#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
int main()
{
		void *mem;
		int id;
		id=shmget(123,1024,0644);
		printf("id: %d\n",id);
		mem=shmat(id,NULL,0);
		printf("res: %s\n",(char *)mem);
		shmdt(mem);
		return 0;
}
