#include <stdio.h>

int main()
{
   int num;
   printf("Enter a num : ");
   scanf("%d", &num);
 int count =0;
 for(int i = 31; i>= 0;i--)
 {
     if((num >> i)&1)
     {
         count++;
     }
 }
 printf("set bits of %d is %d\n", num, count);
    
    
   return 0;
}
