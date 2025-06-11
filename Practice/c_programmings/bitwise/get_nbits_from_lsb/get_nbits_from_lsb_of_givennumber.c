// WAP TO GET N'BITS OF A GIVEN NUMBER FROM LSB ?

#include <stdio.h>

int main()
{
    int num, n;
    scanf("%d %d", &num, &n);
    
    int res = num & ((1<<n)-1);
    printf("The n bits of a given %d number is %d", num, res);
    return 0;
}
