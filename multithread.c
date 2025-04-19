/*
Problem Statement: Multi-Threaded Calculator with File Logging
You need to create a multi-threaded C application using pthread where:

One thread accepts input (two numbers and an operation),

Another thread performs the calculation, and

A third thread writes the result into a file called results.txt.

 Requirements:
Use POSIX threads (pthread_create, pthread_join).

Use shared memory (protected by pthread_mutex_t) or condition variables for synchronization.

Support basic operations: +, -, *, /.

Expected Behavior
User inputs: 5 * 3

Input thread sends data to the calculation thread.

Calculator thread computes the result (15) and passes it to the writer thread.

Writer thread writes the string "5 * 3 = 15" to results.txt.
*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct {

	int a,b,result;
	char op;
	
	
}sharedata;

sharedata sdata ;

void *input_thread(void * arg)
{ 
	printf("enter the operation \n");
	scanf("%d %c %d",&sdata.a,&sdata.op,&sdata.b);
	return NULL;
}

void *cal_thread(void *arg)
{
	switch(sdata.op)
	{	
		case'+':
			sdata.result =sdata.a +sdata.b;
			break;
		
		case'-':
			sdata.result =sdata.a -sdata.b;
			break;
		
		case'*':
			sdata.result =sdata.a *sdata.b;
			break;
		
		case'/':
			sdata.result =(sdata.b != 0 )?(sdata.a /sdata.b): 0;
			break;
		
		default: break;
		 
	}
	
	return NULL;
}

void *write_thread(void *arg)
{
		
	
	FILE * ptr =fopen("results.txt","a");
	fprintf(ptr,"\n %d = %d %c %d \n",sdata.result,sdata.a,sdata.op,sdata.b);
	
	fclose(ptr);
	

	return NULL;
}





int main ()
{
 pthread_t thread1 ,thread2,thread3;
 
 pthread_create(&thread1,NULL,input_thread,NULL);
 pthread_join(thread1,NULL);

  pthread_create(&thread2,NULL,cal_thread,NULL);
  pthread_join(thread2,NULL);
  
  pthread_create(&thread3,NULL,write_thread,NULL);
  pthread_join(thread3,NULL);
  
return 0;
}


