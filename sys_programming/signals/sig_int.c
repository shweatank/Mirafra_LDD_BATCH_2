#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

#if 1
void handler()
{
		printf("press exit key to exit from this\n");
		exit(0);
}
int main()
{
		signal(SIGINT, handler);
		//printf("press exit key to exit from this\n");
        //sleep(1);
		while(1)
		{
				printf("press exit key to exit from this\n");
				//pause();
				sleep(2);
		}
		return 0;
}
#endif

#if 0
int main()
{
		pid_t p=fork();
		if(p==0)
		{
		}
		else if(p>0)
		{
				signal(p,handler);
		}
		else
		{
				perror("failed\n");
				return 1;
		}
		return 0;
}
#endif
