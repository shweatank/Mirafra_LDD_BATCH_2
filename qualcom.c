#include<stdio.h>
int main()
{
		int n1,n2,count=0;
		printf("Enter numbers:\n");
		scanf("%d %d",&n1,&n2);
		for(int i=0;i<=31;i++)
		{
				if(((n2>>i)&1))
				{
						if(!((n1>>i)&1))
						{
								count++;
						}
				}
		}
		printf("res: %d\n",count);
		return 0;
}
