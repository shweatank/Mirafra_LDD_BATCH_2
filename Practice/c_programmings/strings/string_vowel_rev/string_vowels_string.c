// string vowels reverse 
//input :- This is an is india good culture
//output :- This si na si aidni good culture

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int is_vowless(char ch)
{
    ch = tolower(ch);
    if(ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
    {
        return 1;
    }
    return 0;
    
}

int reverse_word(char *str, int s, int e)
{
    e--;
    while(s<e)
    {
        char temp = str[s];
        str[s] = str[e];
        str[e] = temp;
        s++;
        e--;
    }
}
int revesrse_word_vowel(char *str)
{
    int i=0, start = 0;
    int l= strlen(str);
    while(i <= l)
    {
        if(str[i] == ' ' || str[i] == '\0')
        {
            if(is_vowless(str[start]))
            {
                reverse_word(str, start, i);
            }
            start = i+1;
        }
        i++;
    }
}
int main()
{
    char str[100] = "This is an is india with good culture";
    revesrse_word_vowel(str);
    
    printf("The modified string : %s", str);
}
