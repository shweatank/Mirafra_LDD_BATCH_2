#include<stdio.h>
int main()
{
		int max=0,sec=0;
		int arr[5];
		printf("Enter elements:\n");
		for(int i=0;i<5;i++)
		{
				scanf("%d",&arr[i]);
				if(arr[i]>=max)
				{
						sec=max;
						max=arr[i];
				}
				else if(arr[i]<max && arr[i]>=sec)
				{
						sec=arr[i];
				}
				/*if(arr[i]<max && arr[i]>=sec)
				{
						sec=arr[i];
				}*/
		}
		printf("max: %d\n",max);
		//int sec=0;
		/*for(int i=0;i<5;i++)
		{
				if(arr[i]<max && arr[i]>=sec)
				{
						sec=arr[i];
				}
		}*/
		printf("res: %d\n",sec);
		return 0;
}
