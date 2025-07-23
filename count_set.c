#include<stdio.h>
int count=0;
#if 0                //using left shift operator
int main()
{
		int n;
		printf("Enter number:\n");
		scanf("%d",&n);
		for(int i=0;i<32;i++)
		{
				if(n&(1<<i))
				{
						count++;
				}
		}
		printf("count:%d\n",count);
		return 0;
}
#endif
#if 0                 //using right shift operator
int main()
{
		int n;
		printf("Enter number:\n");
		scanf("%d",&n);
		for(int i=0;i<32;i++)
		{
				if(1&(n>>i))
				{
						count++;
				}
		}
		printf("count:%d\n",count);
		return 0;
}
#endif
#if 1
int main()
{
		int n;
		printf("Enter number:\n");
		scanf("%d",&n);
		while(n>0)
		{
				if(n&1)
				{
						count++;
				}
				n=n>>1;
		}
		printf("count=%d\n",count);
		return 0;
}
#endif
