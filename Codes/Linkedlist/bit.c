#include <stdio.h>
#include <stdint.h>
void print(int word){
for(int i=31;i>=0;i--){
printf("%d ", (word>>i)&1);
}
}
int main()
{
	uint32_t word = 0x12345678;
       print(word);
       //1010 1011 
       int start;
       scanf("%d",&start);
       word= ((word&(~(0xF<<start))) |  (10<<start));
       print(word);
}
