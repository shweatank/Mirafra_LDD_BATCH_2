#include<stdio.h>
int main()            //bubble sort
{
		int arr[5];
		printf("Enter elements:\n");
		for(int i=0;i<5;i++)
		{
				scanf("%d",&arr[i]);
		}
		for(int i=0;i<5;i++)
		{
				for(int j=0;j<5-1-i;j++)
				{
						if(arr[j]>arr[j+1])
						{
								int temp=arr[j];
								arr[j]=arr[j+1];
								arr[j+1]=temp;
						}
				}
		}
		for(int i=0;i<5;i++)
		{
				printf("%d ",arr[i]);
		}
		printf("\n");
		return 0;
}
