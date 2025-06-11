// WAP TO IMPLEMENT CIRCUILAR LEFT SHIFT

#include <stdio.h>

int main()
{
    unsigned int num, n;
    scanf("%d %d", &num, &n);
    
    int mask = (1<<n)-1;
    //fetching msb lost bits 
    unsigned int lost = num & (mask << (32-n));
    //doing left shift on num
     num = num << n;
     //setting the msb lost bits to lsb side
     unsigned int res = num | (lost >> (32-n));
     
     for(int i=31; i>=0;i--)
     {
         printf("%d ", (res >> i)&1);
     }
}

