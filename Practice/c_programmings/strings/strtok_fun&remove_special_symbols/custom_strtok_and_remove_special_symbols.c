//Write a C program to split a comma-separated string and remove all special characters from each token.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


int my_isalnum(char c)
{
    if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9')
    {
        return 1;
    }
    return 0;
}
char *my_strtok(char *str, char s)
{
    static char *input = NULL;
    if(str != NULL)
    {
        input = str;
    }
    if(input == NULL || *input == '\0')
    {
        return NULL;
    }
    
    char *start = input;
    while(*input != '\0')
    {
        if(*input == s)
        {
            *input = '\0';
            input++;
            return start;
        }
        input++;
    }
    return start;
}
char remove_spcial_char(char *str)
{
    int j=0;
    int len = strlen(str)+1;
    char temp[len];
    for(int i=0;str[i];i++)
    {
        if(my_isalnum((unsigned char)str[i]))
        {
            temp[j++]  = str[i];
        }
        
    }
    temp[j] = '\0';
    strcpy(str, temp);
}
int main()
{
    char str[] = "vishnu,vardhan$,%@kadavergu,$hi*,viva";
    char *token = my_strtok(str, ',');
    
    while(token != NULL)
    {
        remove_spcial_char(token);
        printf("%s\n", token);
        token = my_strtok(NULL, ',');
    }
}
