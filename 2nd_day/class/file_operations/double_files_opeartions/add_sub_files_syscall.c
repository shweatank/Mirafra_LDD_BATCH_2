// take input from 2 different files and only use system calls to do addition and substarction from the input file values and store the output into another output file 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main()
{
	int num1, num2,sum, diff;
 int f1,f2,f3;

	char buff[100];

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
