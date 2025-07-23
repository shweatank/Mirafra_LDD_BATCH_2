#include<stdio.h>
#if 0
int main()
{
		int arr[5];
		int visited[5]={0};
		printf("Enter elements:\n");
		for(int i=0;i<5;i++)
		{
				scanf("%d",&arr[i]);
		}
		int count=0;
		for(int i=0;i<5;i++)
		{
				if(visited[i]==1)
				{
						continue;
				}
				for(int j=i+1;j<5;j++)
				{
						if(arr[i]==arr[j])
						{
								visited[j]=1;
								count++;
						}
				}
				if(count>0)
				{

				printf("element is %d\n",arr[i]);
				printf("number of times: %d\n",count);
				}
				count=0;
		}
		return 0;
}
#endif
#if 1
int main()
{
		int n=5;
		int arr[n];
		printf("Enter elements:\n");
		for(int i=0;i<n;i++)
		{
				scanf("%d",&arr[i]);
		}
		for(int i=0;i<n;i++)
		{
				for(int j=i+1;j<n;j++)
				{
						if(arr[i]==arr[j])
						{
								for(int k=j;k<n-1;k++)
								{
										arr[k]=arr[k+1];
								}
								n--;
								j--;
						}
						//n--;
						//j--;
				}
		}
		for(int i=0;i<n;i++)
		{
				printf("%d ",arr[i]);
		}
		printf("\n");
		return 0;
}
#endif
