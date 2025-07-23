#include<stdio.h>
int main()
{
		int n,count=0;
		printf("Enter number:\n");
		scanf("%d",&n);
		for(int i=2;i<=n/2;i++)
		{
				if(n%i==0)
				{
						printf("not prime\n");
						count++;
						break;
				}
		}
		if(count==0)
		{
				printf("prime\n");
		}
		return 0;
}
