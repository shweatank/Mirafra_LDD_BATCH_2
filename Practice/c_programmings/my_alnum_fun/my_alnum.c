// WAP TO CHECK THE GIVEN CHAR IS ALNUM OR NOT 

#include <stdio.h>

int main()
{
    char ch;
    scanf("%c", &ch);
    
    if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9')
    {
        printf("This is a alnum character");
    }
    else
    {
        printf("not alnum");
    }
    return 0;
}

