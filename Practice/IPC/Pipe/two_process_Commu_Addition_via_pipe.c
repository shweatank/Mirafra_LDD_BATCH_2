// Two process communicates via pipes and do Addition operations
// a. Parent process take inputs from user and  send to child 
// b. Child process takes the inputs from parent and do Sum of two numbers


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
	int fd1[2], fd2[2];
	
	if(pipe(fd1) == -1 || pipe(fd2) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	 
	pid_t pid = fork();
	if(pid <0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if(pid > 0)
	{
		//parent orocess
		close(fd1[0]);
		close(fd2[1]);

		int num1, num2;
		printf("Enter the num1 and num2 values : \n");
		scanf("%d %d", &num1, &num2);

		write(fd1[1], &num1, sizeof(num1));
		write(fd1[1], &num2, sizeof(num2));

		int result;
		read(fd2[0], &result, sizeof(result));

		printf("The sum of num1 %d and num2 %d is %d\n", num1, num2, result);
		close(fd1[1]);
		close(fd2[0]);

		wait(NULL);
	}
	else 
	{
		//child process

		close(fd1[1]);
		close(fd2[0]);

		int a, b, res;

		read(fd1[0], &a, sizeof(a));
		read(fd1[0], &b, sizeof(b));

		res = a + b;
		write(fd2[1], &res, sizeof(res));

		close(fd1[0]);
		close(fd2[1]);

		exit(0);
	}
}
