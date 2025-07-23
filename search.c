#include<stdio.h>
#if 0
int main()   //linear search
{
		int n;
		printf("Enter element to search:\n");
		scanf("%d",&n);
		int arr[5];
		printf("Enter array:\n");
		for(int i=0;i<5;i++)
		{
				scanf("%d",&arr[i]);
		}
		for(int i=0;i<5;i++)
		{
				if(n==arr[i])
				{
						printf("ele is %d\n",arr[i]);
						break;
				}
		}
		//printf("element is not present\n");
		return 0;
}
#endif

#if 1
int main()          //binary search
{
		int n;
		printf("Enter element to search:\n");
		scanf("%d",&n);
		int arr[5];
		printf("Enter array:\n");
		for(int i=0;i<5;i++)
		{
				scanf("%d",&arr[i]);
		}
		int mid;
		int f=0;
		int l=4;
		while(f<=l)
		{
				mid=(f+l)/2;
				if(n==arr[mid])
				{
						printf("ele is %d\n",arr[mid]);
						break;
				}
				else if(n>arr[mid])
				{
						f=mid+1;
				}
				else
				{
						l=mid-1;
				}
		}
		printf("data not found\n");
		return 0;
}
#endif
