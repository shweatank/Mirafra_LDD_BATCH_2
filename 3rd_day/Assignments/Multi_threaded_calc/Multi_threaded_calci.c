#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <error.h>

int ready1=0, ready2 = 0;
int num1, num2,res=0, fd;
char op;
char str[100];
void *user_inputs(void *args)
{
	printf("Enter the user inputs : \n");
	scanf("%d %c %d",&num1, &op, &num2);
	ready1 = 1;

}
void *operation(void *args)
{
	while(ready1 ==0)
	{
	}
	switch(op)
	{
		case '+':
			res = num1 + num2;
			break;
		case '-':
			res = num1 - num2;
			break;
		case '*':
			res = num1 * num2;
			break;
		case '/':
			res = num1 / num2;
			break;
		default :
			printf("Invalid opearations");
			break;
	}
	ready2 = 1;
}

void *file_operation(void *args)
{
	while(ready2 ==0);
	if(ready2 > 0){
	int l = snprintf(str,sizeof(str),"The result is = %d\n",res);
	if(l<0)
	{
		perror("failed to store the result into the string \n");
	//	return 1;
	}
	fd = open("result.txt", O_WRONLY | O_CREAT| O_TRUNC, 0644);
	if(fd < 0)
	{	
		perror("Failed to open result.txt file");
//		return 1;

	}
	l = write(fd, str, l);
	if(l<0)
	{
		perror("Failed to store the data into a string \n");
//		return 1;
	}
	else
	{
		printf("The output data is store into the result.txt file\n");
	
	}
	close(fd);
}
}

int main()
{
	pthread_t thread1, thread2, thread3;
	int iter1,iter2,iter3;

	iter1 = pthread_create(&thread1, NULL, user_inputs, NULL);
	if(iter1 < 0)
	{
		perror("Thread1 creation failed");
		return 1;
	}

	iter2 = pthread_create(&thread2, NULL, operation, NULL);
	if(iter2 < 0)
	{
		perror("Thread2 creation failed");
		return 1;
	}
	iter3 = pthread_create(&thread3, NULL, file_operation, NULL);
	if(iter3 < 0)
	{
		perror("Thread3 creation failed");
		return 1;
	}

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	return 0;

}
