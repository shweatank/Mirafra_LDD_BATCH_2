#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int  num1, num2,sum, diff;
int f1,f2,f3;
size_t bytesread;
char buff[100];

int user_inputs()
{
	f1 = open("input1.txt", O_RDONLY);
	if(f1 < 0)
	{
		perror("open input1.txt file  failure \n");
		return 1;
	}
	size_t bytesread = read(f1, buff, sizeof(buff));
	if(bytesread < 0)
	{
		perror("Reading input1.txt file failure \n");
		return 1;
	}
	printf("bytes %ld\n", bytesread);
	buff[bytesread] = '\0';
	num1 = atoi(buff);
	close(f1);

	f2 = open("input2.txt", O_RDONLY);
	if(f2 < 0)
	{
		perror("open input2.txt file  failure \n");
                return 1;
	}
	bytesread = read(f2, buff, sizeof(buff));
	if(bytesread < 0)
        {
                perror("Reading input2.txt file failure \n");
                return 1;
        }

        buff[bytesread] = '\0';
        num2 = atoi(buff);
        close(f2);
	printf("num1 = %d num2 = %d\n", num1, num2);
}

int add_sub()
{
	sum = num1 + num2;
	diff = num1- num2;

	bytesread = snprintf(buff, sizeof(buff),"num1 = %d\nnum2 = %d\nSum = %d\nDifference = %d\nAddition value = %d, Substrcation value = %d\n",num1,  num2, sum, diff, sum, diff);
	if(bytesread < 0)
	{
		perror("Reading format input to a string is failed \n");
		return 1;
	}

	f3 = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(f3 < 0)
        {
                perror("open output.txt file  failure \n");
                return 1;
        }


	bytesread = write(f3, buff, sizeof(buff));
	if(bytesread < 0)
	{
		perror("writing to the output.txt file failed");
	       return 1;	
	}

	printf("The output is written to the output.txt file! \n");

	bytesread = read(f3, buff, strlen(buff));
        if(bytesread < 0)
        {
                perror("Reading output.txt file failure \n");
                return 1;
        }

        buff[bytesread] = '\0';
	printf("%s",buff);
	close(f3);
}

int main()
{
        pid_t pid = fork(); // create a new process
        if(pid < 0)
        {
        //printf("pid is less than 0\n");
        //fork failed
        perror("fork");
        exit(EXIT_FAILURE);
        }
        else if(pid ==0)
        {
        //printf("Pid value is equal to 0\n");

        //child process
        printf("Child process (PID: %d) is running 'ls -l'\n", getpid());
	add_sub();
        execlp("ls", "ls", "-l", (char *)NULL);
        
        // if exec fails
        perror("execlp");
        exit(EXIT_FAILURE);
        }
        else
        {
        //parent process
        printf("Parent process (PID: %d) is waiting for child...\n", getpid());
	user_inputs();
        wait(NULL) ;// wait for child to finish
        printf("Child process finished. parent exiting.\n");
        }
}

