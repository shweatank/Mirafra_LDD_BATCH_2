// WAP TO IMPLEMENT CIRCUILAR RIGHT SHIFT

#include <stdio.h>

int main()
{
    unsigned int num, n;
    scanf("%d %d", &num, &n);
    
    //get the nbits form the lsb to lost
    int mask2 = (1<<n)-1;
    int lost = num & mask2;
    
    //replace the lsb lost data to msb of clear place
    num = num >> n;
    num = num | (lost << (32-n));
    
    for(int i = 31; i>=0;i--)
    {
        printf("%d ", (num >> i)&1);
    }
    return 0;
}

