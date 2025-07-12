//Write a C program to split a comma-separated string and remove all special characters from each token.


#include <stdio.h>
#include <string.h>
#include <ctype.h>
char remove_special_char(char *src)
{
    int j=0;
    int len = strlen(src)+1;
    char temp[len];
    for(int i=0; src[i] != '\0'; i++)
    {
        if(isalnum((unsigned char)src[i]))
        {
            temp[j++] = src[i];
        }
    }
    temp[j] = '\0';
    strcpy(src, temp);
}
int main()
{
   char str[] = "vishnu,vardhan$,%@kadavergu,$*hi,viva" ;
   char *token = strtok(str, ",");
   
   while(token != NULL)
   {
       remove_special_char(token);
       printf("%s\n", token);
       token = strtok(NULL, ",");
   }
}
