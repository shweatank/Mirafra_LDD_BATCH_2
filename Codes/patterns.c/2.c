#include <stdio.h>

int main()
{
	int i,j,num;
	printf("Enter num :");
	scanf("%d",&num);

	for(i=num;i>=1;i--)
	{
		for(j=num;j>=1;j--)
		{
			printf("* ");

		}
		printf("\n");
	}
	return 0;
}
