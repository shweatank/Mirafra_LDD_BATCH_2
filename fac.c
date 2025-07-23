#include<stdio.h>
#if 0
int main()
{
		int n;
		printf("Enter number:\n");
		scanf("%d",&n);
		int fact=1;
		//int t=0;
		while(n>0)
		{
				fact=n*fact;
				n--;
		}
		printf("%d\n",fact);
		return 0;
}
#endif

#if 1
int fun(int n)
{
		int f=0;
		if(n==0)
		{
				return 1;
		}
		else
		{
				f=n*fun(n-1);
		}
		return f;
}
int main()  //using recursion
{
		int n;
		printf("Enter number:\n");
		scanf("%d",&n);
		int d=fun(n);
		printf("%d\n",d);
		return 0;
}
#endif
