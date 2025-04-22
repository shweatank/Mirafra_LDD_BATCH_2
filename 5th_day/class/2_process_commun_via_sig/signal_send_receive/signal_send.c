#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main()
{
	pid_t pid;
	printf("Enter PID to send SIGUSR1: ");
	scanf("%d", &pid);

	kill(pid, SIGUSR1);
	printf("Sent SIGUSR1 to PID %d\n", pid);
	return 0;
}
