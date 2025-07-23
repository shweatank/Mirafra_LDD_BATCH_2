#include<stdio.h>
#if 1                          //replace number of bits from lsb
int main()
{
		int n,v,b;
		printf("Enter number:");
		scanf("%d",&n);
		printf("Enter value to replce:");
		scanf("%d",&v);
		printf("Enter number of bits to replace:");
		scanf("%d",&b);
		n=n&(~((1<<b)-1));
		printf("%d\n",n);
		n=n|v;
		printf("result %d\n",n);
		return 0;
}
#endif

#if 0
int main()
{
		int n,b;
		printf("Enter number:");
		scanf("%d",&n);
		printf("number of bits to print from lsb:");
		scanf("%d",&b);
		n=n&((1<<b)-1);
		printf("result %d\n",n);
		return 0;
}
#endif
