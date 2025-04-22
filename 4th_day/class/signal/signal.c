#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig)
{
	printf("\nCaught signal %d (SIGINT), Existing gracefully. \n", sig);
	exit(0);
}

int main()
{
	signal(SIGINT, handle_sigint); // catch ctrl+c
	while(1) {
		printf("Running... Press Ctrl+c to stop.\n");
		sleep(1);
	}
}

