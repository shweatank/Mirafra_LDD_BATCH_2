#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig)
{
	printf("Hnadled signals %d using sigaction\n", sig);
}

int main()
{
	struct sigaction sa;
	sa.sa_handler = sig_handler;
	sa.sa_flags = 0; // SA_RESTART is also common
	sigemptyset(&sa.sa_mask);

	sigaction(SIGINT, &sa, NULL);
	while(1)
	{
		printf("Waiting for SIGINT...\n");
		sleep(2);
	}
}
