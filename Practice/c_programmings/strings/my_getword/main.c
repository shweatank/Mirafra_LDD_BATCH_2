//WAP TO GETWORD FUNCTION

#include <stdio.h>
int my_getword(char *str)
{
    int count =0;
   for(int i=0;str[i] != ' ' && str[i] != '\0' ;i++)
   {
       count++;
   }
   str[count] = '\0';
   return count;
}
int main()
{
    char str[100];
    printf("Enter a string : \n");
    scanf("%[^\n]", str);
    
    int len = my_getword(str);
    printf("The legth of your getword %s is %d", str, len);
}
