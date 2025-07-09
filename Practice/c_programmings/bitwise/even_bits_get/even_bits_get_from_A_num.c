#include <stdio.h>

int main()
{
    int num;
    scanf("%d",&num);
    int mask =0;
    for(int i=1;i<15;i+= 2)
    {
      mask = mask | (1 << i);  
    }
    num = num & mask;
    
    for(int i=15; i>=0;i--)
    {
        printf("%d ", (num >> i)& 1);
    }
}
