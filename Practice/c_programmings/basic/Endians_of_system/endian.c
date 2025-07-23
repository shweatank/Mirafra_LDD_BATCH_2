//endians data representation

#include <stdint.h>
#include <stdio.h>

int main()
{
    uint32_t a = 0x12345678;  //msb = 12, lsb = 78 
    uint8_t *ptr = (uint8_t *)&a;
    for(int i=0;i<sizeof(a);i++)
    {
        printf("ptr[%d] 0x%x\n", i, ptr[i]);
    }
    if(ptr[0] == 0x78)
    {
        printf("little endians");  //lsb in lowest address(starting address) = Littile endians
    }
    else if(ptr[0] == 0x12)  //->msb in lowest(Starting address) = big endians
    {
        printf("Big endians");
    }
}

