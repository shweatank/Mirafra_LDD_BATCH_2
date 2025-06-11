#include <stdio.h>
int prime(int n)
{
    int c=0;
    for(int i=2; i <= n/2;i++)
    {
        if(n % i == 0)
        {
            c=1;
            break;
        }
    }
    if(c ==1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int main()
{
   int num, n, m;
   printf("Enter a num : ");
   scanf("%d", &num);
   
    for(int i=2; i<= num;i++)
    {
        int m = prime(i);
        if(!m)
        {
            printf("%d ", i);
        }
    }
    
   return 0;
}
