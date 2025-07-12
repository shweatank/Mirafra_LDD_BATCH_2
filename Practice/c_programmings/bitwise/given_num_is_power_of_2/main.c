//check the given number is power of 2 using Bitwise operator

#include <stdio.h>

int main()
{
    int n;
    printf("Enter num : ");
    scanf("%d", &n);
    
    if(((n & (n-1)) == 0) && (n != 0))
    {
        printf("The given number %d is power of 2\n", n);
    }
    else
    {
        printf("The given number %d is not power of 2\n", n);
    }
    return 0;
}
