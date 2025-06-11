//WAP TO IMPEMENT CIRCUILAR RIGHT SHIFT

#include <stdio.h>

int main()
{
    unsigned int num;
    int n;

    scanf("%u %d", &num, &n);

    // Ensure n is within 0–31
    n = n % 32;

    unsigned int result = (num >> n) | (num << (32 - n));

    // Print result in binary
    for (int i = 31; i >= 0; i--)
    {
        printf("%d ", (result >> i) & 1);
    }

    printf("\nResult = %u\n", result);

    return 0;
}

