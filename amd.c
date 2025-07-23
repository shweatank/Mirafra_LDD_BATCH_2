#include<stdio.h>
#if 0                      //printing number in binary form
int main()
{
		int n;
		printf("Enter number\n");
		scanf("%d",&n);
		for(int i=31;i>=0;i--)
		{
				if((n>>i)&1)
				{
						printf("1");
				}
				else
				{
						printf("0");
				}
		}
		printf("\n");
		return 0;
}
#endif

#if 0                          //counting consective 1's in number
int main()
{
		int n;
		int max=0,count=0;
		printf("Enter number:\n");
		scanf("%d",&n);
		for(int i=0;i<=31;i++)
		{
				if((n>>i)&1)
				{
						count++;
				}
				else
				{
						count=0;
				}
				if(count>=max)
				{
						max=count;
				}
		}
		printf("max: %d\n",max);
		return 0;
}
#endif

#if 0                      //atoi function
int main()
{
		char str[20];
		printf("Enter string:\n");
		scanf("%s",str);
		int num;
		int ret=sscanf(str,"%d",&num);     //on success it returns 1 of failure -1
		if(ret==1)
		{
				printf("result: %d\n",num);
		}
		else
		{
				printf("0\n");
		}
		return 0;
}
#endif

#if 1             //mask the bits
int main()
{
		return 0;
}
#endif
