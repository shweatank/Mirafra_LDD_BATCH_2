#include <stdio.h>
#include "add.h"

int main()
{
	int num1, num2;
	scanf("%d %d", &num1, &num2);
	printf("the sum is %d\n", add(num1,num2));
	return 0;
}
