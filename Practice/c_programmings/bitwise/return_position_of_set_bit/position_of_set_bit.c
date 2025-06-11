//WAP TO RETURN THE SET BIT POSITIONS FROM A GIVEN NUMBER

#include <stdio.h>
int find_set_bit(int num)
{
    printf("The set bit positions : ");
    for(int i=0;i<32;i++)
    {
        if(num & (1<<i))
        {
            printf("%d ", i);
        }
    }
    return 0;
}
int main()
{
    int num;
    printf("Enter a num : \n");
    scanf("%d", &num);
    
    find_set_bit(num);
}
