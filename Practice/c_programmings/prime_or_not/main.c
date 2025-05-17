#include <stdio.h>

int main()
{
   int num;
   printf("Enter a num : ");
   scanf("%d", &num);
   int c=0;
    for(int i=2; i<= num/2;i++)
    {
        if(num % i ==0)
        {
            c=1;
            break;
        }
    }
    if(c ==0)
    {
        printf("prime number");
    }
    else
    {
        printf("not, prime number");
    }
    
   return 0;
}
