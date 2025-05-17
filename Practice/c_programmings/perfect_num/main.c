#include <stdio.h>

int main()
{
   int num;
   printf("Enter a num : ");
   scanf("%d", &num);
   int sum = 0;
   for(int i=1; i< num; i++)
   {
       if(num % i == 0)
       {
           sum += i;
       }
   }
   if(num == sum)
   {
       printf("perfect num\n");
   }
   else
   {
       printf("Not perfect num\n");
   }
   return 0;
}
