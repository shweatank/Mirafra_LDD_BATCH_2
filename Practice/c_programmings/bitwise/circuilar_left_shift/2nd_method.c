#include <stdio.h>

int main()
{
    unsigned int num, n;
    scanf("%u %u", &num, &n);

    n = n % 32; // ensure valid shift

    unsigned int result = (num << n) | (num >> (32 - n));

    for (int i = 31; i >= 0; i--)
    {
        printf("%d ", (result >> i) & 1);
    }
    printf("\n");

    return 0;
}

