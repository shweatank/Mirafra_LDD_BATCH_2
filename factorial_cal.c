#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/mman.h>
//int res[3];
int factorial(int n)
{
		int fact=1;
		while(n>0)
		{
				fact=fact*n;
				n--;
		}
		return fact;
}
int main()
{
		int arr[3]={1,2,3};
		int *res = mmap(NULL, 3 * sizeof(int),
						                    PROT_READ | PROT_WRITE,
											                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		//static int res[3]={0};
		for(int i=0;i<3;i++)
		{
				pid_t p=fork();
				if(p==0)
				{
				   res[i]=factorial(arr[i]);
				  // perintf("%d ",res[i]);
				   exit(0);
				}
				if(p>0)
				{
						wait(NULL);
				}
		}
		for(int i=0;i<3;i++)
		{
				printf("%d ",res[i]);
		}
		printf("\n");
		munmap(res,3*sizeof(int));
		return 0;
}
		
