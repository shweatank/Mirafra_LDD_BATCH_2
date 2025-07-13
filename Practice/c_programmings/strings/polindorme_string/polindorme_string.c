#include <stdio.h>
#include <string.h>

int main()
{
    char str[] = "abcba";
    int len = strlen(str);
    int start = 0, end = len-1;
    while(start < end)
    {
        if(str[start++] != str[end--])
        {
            printf("not");
            return 0;
        }
    }
    printf("polindorme");
    return 0;
}
