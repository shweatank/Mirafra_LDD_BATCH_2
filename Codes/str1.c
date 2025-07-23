#include <stdio.h>

void reverse(char *str, int start, int end) 
{
    char temp;
    while(start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int main() 
{
    char str[] = "my name is";
    int i = 0, start = 0;

    while(str[i] != '\0') 
    {	
	    i++;
    }
    reverse(str, 0, i - 1);

    i = 0;
    while(str[i] != '\0') 
    {
        if(str[i] != ' ' && str[i] != '\0') 
	{
            start = i;
            while(str[i] != ' ' && str[i] != '\0') 
	    {
		    i++;
	    }
            reverse(str, start, i - 1);
        }
       	else
       	{
            i++;
        }
    }

    printf("%s\n", str);

    return 0;
}

