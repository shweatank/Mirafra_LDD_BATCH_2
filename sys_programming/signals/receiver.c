#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void handler(int sig)
{
		printf("bye\n");
		printf("rec %d\n",sig);
		exit(0);
}
int main()
{
		signal(SIGUSR1,handler);
		while(1)
		{
				pause();
		}
		return 0;
}
