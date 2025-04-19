/*
Custom Sort with Function Pointers
Problem Statement:
Implement a generic sorting function (similar to qsort) that accepts an array and a comparator function pointer to sort the array in different ways (ascending, descending, even-first, etc.).

Skills Used: Comparator functions, function pointer parameters.

 qsort(arr, n, sizeof(int), compare);
*/

#include<stdio.h>


int assending(int a, int b)
{
	
	return a-b;
	
}
int decending(int a, int b)
{
	return b-a;
}

int evenfirst(int a, int b)
{	if((a%2 ==0)&&(b%2 != 0)){return -1;}
	if((a%2 !=0)&&(b%2 == 0)){return 1;}
	return  a-b;
}


void customsort(int *arr,int size ,int (*ptr)(int,int))
{
	for(int i = 0 ;i<size;i++)
	{	
		
		for(int j= i+1;j<size;j++)
		{	
			if((ptr(arr[i],arr[j])>0))
			{
				int temp = arr[i];
				arr[i] =arr[j];
				arr[j]=temp;
				
			}
		}
	}
	
		for(int j= 0;j<size;j++)
		{	
			printf("%d",arr[j]);
			
		}
	printf("\n");
  	
}
int main()
{	
	int size = 7;
	int arr[]={1,4,2,5,3,7,6};
	
		for(int j= 0;j<size;j++)
		{	
			printf("%d",arr[j]);
			
		}
		printf("\n");
	customsort(arr,size,assending);
	customsort(arr,size,decending);
	customsort(arr,size,evenfirst);
	
	return 0;
}
