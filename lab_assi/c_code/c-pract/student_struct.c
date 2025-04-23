
//calculating student individual marks and total marks of student

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define N 5

struct student
{
	char name[20];
	int roll;
	int marks[N];
};
int main()
{       int sum,total=0;
	struct student arr[N];
	for(int i=0;i<N;i++)
	{
		 sum=0;

		printf("enter the student name:\n");
		scanf("%s",arr[i].name);
		printf("enter the student roll no:\n");
		scanf("%d",&arr[i].roll);
		printf("enter the student marks:\n");

		for (int j=0;j<N;j++)
		{       
			scanf("%d",&arr[i].marks[j]);
		}
		for(int k=0;k<N;k++)
		{
			sum=sum+arr[i].marks[k];
			total=total+arr[i].marks[k];
		
		}
		printf("sum of marks of single student=%d\n",sum);
		printf("total of all the student marks=%d\n",total);
	}
	
	
	return 0;
}

