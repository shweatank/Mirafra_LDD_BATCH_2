// WAP TO PUT THE (B-A+1)LSB'S OF NUM INTO VAL(B-A)

#include <stdio.h>

int main()
{
    int num, val, a,b;
    scanf("%d %d %d %d", &num, &a, &b,&val);
   //mask 
   int n = (b-a)+1;
   
   int mask = (1<<n)-1;
   mask = mask << a;
   mask = ~mask;
   
   //clear the nbits from pos of b
   val = val & mask;
   
   //fetch nbits from lsb of num
   int mask2 = (1<<n)-1;
   int get = num & mask2;
   //replace nbits of fetched to val
   get  = get<< a;
   int g = get | val;
   
   printf("%d", g);
}


