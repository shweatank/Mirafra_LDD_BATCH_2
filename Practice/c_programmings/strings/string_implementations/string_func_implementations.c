#include <stdio.h>
#include <string.h>
int mystrlen(char *str)
{
    int size=0;
    for(int i=0;str[i] != '\0';i++)
    {
        size++;
    }
    return size;
}
int my_strcmp(char *str1, char *str2)
{
    while(*str1 && *str2 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

int my_strcpy(char *dest, char *src)
{
    while(*dest++ = *src++);
    return *dest;
}

char *my_strstr(char *str, char *s)
{
    if(!*s)
    {
        return str;
    }
    for(int i=0; str[i];i++)
    {
        int j=0;
        while(s[j] && str[i+j] == s[j])
        {
            j++;
        }
        if(s[j] == '\0')
        {
            return &str[i];
        }
    }
    return NULL;
}

char *my_strtok(char *str,const char *delim)
{
   static char *next= NULL;
    if(str != NULL)
    {
      next = str;  
    }
    while(*next && delim == next)
    {
        next++;
    }
    
        if(*next == '\0')
        {
            return NULL;
        }
    
    char *start = next;
    while(*next && !strchr(delim, *next))
    {
        next++;
    }
    if(*next)
    {
        *next = '\0';
        next++;
    }
    else
    {
        return NULL;
    }
    return start;
}
int main()
{
    char str1[] = "hello world hwo are you";
    int size = mystrlen(str1);
    printf("size of the %s is %d\n", str1, size);
    char str2[] = "hello world hwo are you";
    if(!my_strcmp(str1, str2))
    {
        printf("both the strings are equal\n");
    }
    else
    {
        printf("both the strings are not equal\n");
    }
    
    char str3[50];
    my_strcpy(str3, str1);
    printf("The copied string str3 is = %s\n", str3);
    char str4[] = "world";
     char *res =  my_strstr(str3, str4);
    printf("found at :  %s\n", res);
    
    char str[] = "apple,banana;grape|orange";
    const char *delimiters = ",;|";

    char *token = my_strtok(str, delimiters);

    while (token) {
        printf("Token: %s\n", token);
        token = my_strtok(NULL, delimiters);
    }
    
}
