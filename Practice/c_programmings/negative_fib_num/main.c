#include <stdio.h>

int main()
{
   int num;
   printf("Enter the num : ");
   scanf("%d", &num);
   
   int a =0, b = 1, next = 0;
   while(a > num)
   {
       printf("%d ", a);
       next = a - b;
       a = b ;
        b = next;
   }
   return 0;
}
