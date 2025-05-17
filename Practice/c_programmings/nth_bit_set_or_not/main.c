//to check nth bot is set or not if set clear mth bit
#include <stdio.h>

int main()
{
   int num, n, m;
   printf("Enter a num n m: ");
   scanf("%d %d %d", &num, &n ,&m);
   int mask = 1<< n;
   if(num & mask)
   {
       num = num & (~(1<<m));
       printf("updated num %d", num);
   }
    
    
   return 0;
}
