//WAP TO REPLACE EACH STRING OF ONE OR MORE BLANKS BY A SINGLE BLANKS

#include <stdio.h>
char* space_string(char *str)
{
   char output[100];
   int i=0,j=0, is_space=0;
   while(str[i] != '\0')
   {
       if(str[i] == ' ' || str[i] == '\t')
       {
           if(!is_space)
           {
               output[j++] = ' ';
               is_space = 1;
           }
       }
       else
       {
           output[j++] = str[i];
           is_space =0;
       }
        i++;
   }
   output[j] = '\0';
   printf("the output is %s\n", output);  //printing output in subfunction
   
   //copyting the output into str stringfor return back to print on main function
   
   i=0;
   while(output[i] != '\0')
   {
       str[i] = output[i];
       i++;
   }
   str[i] = '\0';
   return str;
   
  
}
int main()
{
    char str[100];
    printf("Enter the string : \n");
    scanf("%[^\n]", str);
    space_string(str);
    printf("the output result in main function %s\n", str);     //Printing output in main function 
    
}
