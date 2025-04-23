#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


 int main()
 {
     char ch;
     int cha=0,line=0,word=0;
     int fb=open("input.txt",O_RDONLY);
     if(fb<0)
     {
         printf("failed to open file\n");
         return 0;
     }
     while(read(fb,&ch,1)==1)
     {
         cha++;
         if(ch=='\n')
         {
             line++;
         }
         if(isspace(ch))
         {
             word++;
         }
     }
     close(fb);
     printf("no of character=%d\n",cha);
     printf("no of lines in a file =%d\n",line);
     printf("no of words in a file=%d\n",word);
     return 0;
 }
 
