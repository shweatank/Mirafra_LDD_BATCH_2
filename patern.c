#include<stdio.h>
#if 0
int main()
{
		int n;
		printf("Enter n:\n");
		scanf("%d",&n);
		for(int i=0;i<n;i++)
		{
				for(int j=i;j<n-1;j++)
				{
						printf(" ");
				}
				for(int k=0;k<=i;k++)
				{
						printf("* ");
				}
				printf("\n");
		}
		for(int i=0;i<n-1;i++)
		{
				for(int j=0;j<=i;j++)
				{
						printf(" ");
				}
				for(int k=i;k<n-1;k++)
				{
						printf("* ");
				}
				printf("\n");
		}
		return 0;
}
#endif
#if 0
int main()
{
		int n;
		printf("Enter n:\n");
		scanf("%d",&n);
		for(int i=0;i<n;i++)
		{
				/*for(int j=0;j<=i;j++)
				{

				printf("* ");
				}*/
				for(int j=0;j<n;j++)
				{
						printf("* ");
				}
				printf("\n");
		}
		return 0;
}
#endif

#if 1
int main()
{
		int row=5;
		int col=8;
		for(int i=0;i<row;i++)
		{
				for(int j=0;j<col;j++)
				{
						if(i==0 || j==0 || i==row-1 || j==col-4)
						{
								printf("*");
						}
						else
						{
								printf(" ");
						}
				}
				printf("\n");
		}
		printf("\n");
		return 0;
}
#endif
