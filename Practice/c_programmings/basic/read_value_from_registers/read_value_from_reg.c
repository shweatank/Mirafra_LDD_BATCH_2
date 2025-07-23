//fetch value from the registers eg:- 0x30003000 for pc not exact address

#include <stdint.h>
#include <stdio.h>

int main()
{
    volatile uint32_t dummy;
    volatile uint32_t *ptr = &dummy;
    *ptr= 10;
    volatile uint32_t value = *ptr;
    printf("the value %d\n", value);
}

/* for exact address 0x3003000 on embedded mcu registers address

#include <stdio.h>
#include <stdint.h>

int main()
{
    volatile uint32_t *ptr = (volatile uint32_t *)0x30003000;
    volatile uint32_t value = *ptr;
    printf("%u", value);
    return 0;
}

*/

