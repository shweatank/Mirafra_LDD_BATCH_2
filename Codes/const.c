#include <stdio.h>

int main()
{
    const int num = 10;
    printf("%d\n", num);

    int *ptr = &num;
    *ptr = 11;
    printf("%d\n", num);
}

