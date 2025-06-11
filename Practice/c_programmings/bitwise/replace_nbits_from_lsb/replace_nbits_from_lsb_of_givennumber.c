// WAP TO REPLACE NBITS FROM GIVEN NUMBER ?

#include <stdio.h>

int main()
{
    int num, n,val;
    scanf("%d %d %d", &num, &n, &val);
    
    //clear nbits of num 
    int mask = (1<<n)-1;
    mask = ~mask;
    num  = num & mask;
    
    //get nbits from val 
    int res = val & ((1<<n)-1);
    
    //merge res to num 
    num = num | res;
    
    printf("the updated num value is %d", num);
    return 0;
}
