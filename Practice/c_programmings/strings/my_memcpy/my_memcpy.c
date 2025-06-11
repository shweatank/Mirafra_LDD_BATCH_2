//WAP TO IMPLEMENT OWN MEMCPY FUNCTION
//memcpy :- Custom memcpy function: This function copies n bytes from source to destination memory.
#include <stdio.h>

void* my_memcpy(void *dest, void *src, int len)
{
    char *d = (char *)dest;
    char *s = (char *)src;
    
  for(int i=0; i < len; i++)
  {
      d[i] = s[i];
  }
}
int main()
{
    char src[] = "Hello world";
    char dest[20];
    
    my_memcpy(dest, src, sizeof(src));
    printf("After copied the string %s", dest);
}
