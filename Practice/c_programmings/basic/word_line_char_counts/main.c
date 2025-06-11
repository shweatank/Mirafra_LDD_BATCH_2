//WAP TO COUNT NO. OF CHARACTERS, WORDS AND LINES ENTERED THROUGH STDIN

#include <stdio.h>

int main()
{
    int char_count =0;
    int line_count =0;
    int word_count =0;
    int current_char ;
    int prev_char = ' ';
    
    while((current_char = getchar()) != EOF)
    {
        char_count++;
        if((current_char == ' ' || current_char == '\t' || current_char == '\n') && (prev_char != ' '|| prev_char != '\t' || prev_char != '\n'))
        {
            word_count++;
        }
        if(current_char == '\n')
        {
            line_count++;
        }
        prev_char = current_char;
    }
    if(prev_char != ' ' && prev_char != '\t' && prev_char != '\n')
    {
        word_count++;
    }
    if(prev_char != '\n')
    {
        line_count++;
    }
    
    
    printf("The character count %d\n", char_count);
    printf("The word count %d\n", word_count);
    printf("The line count %d\n", line_count);
}
