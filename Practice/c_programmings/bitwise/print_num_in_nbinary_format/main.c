// WAP TO PRINT N BITS FROM LSB OF A NUMBER ?
//example 10 is a number and 12 is a bits, so i need to print it on 10 in 12 bits 
#include <stdio.h>

int main()
{
    int num, val;
    scanf("%d %d", &num, &val);
    
   for(int i = 12 ; i >=0 ; i--)
   {
       printf("%d ", (num >> i)&1);
   }
}
