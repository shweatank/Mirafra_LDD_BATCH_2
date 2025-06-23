#include <stdio.h>

int main()
{
	unsigned int num = 10;
	int j = 8, i = 5, result;
	
	unsigned int mask = ((1 << (j - i + 1)) - 1) << i;
	result = num | mask;

	printf("Original 0x%X\n", num);
	printf("Modified 0x%X\n", result);

	return 0;
}
