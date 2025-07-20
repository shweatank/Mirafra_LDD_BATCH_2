#include <stdio.h>
int cons(int n)
{
    while(n != 0)
    {
        int next =0,current  = 0;
        if(n&1)
        {
            current = 1;
            n = n >> 1;
            if(n & 1)
            {
                next = 1;
                printf("consecuative 1's found %d %d\n", current, next);
                return 1;
            }
        }
        n = n >> 1;
    }
    return 0;
}
int main()
{
    int num;
    scanf("%d", &num);
    if(cons(num))
    {
        printf("consecuative");
    }
    else
    {
        printf("not");
    }
}
