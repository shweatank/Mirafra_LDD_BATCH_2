#include <stdio.h>
#include <string.h>


int main()
{
  FILE *fp;

  const char *message  = "HI \nI am Keerthi";
  const char *filename = "res.txt";
  

  fp = fopen(filename,"w");

  if(fp == NULL)
  {
    printf("Error Occcured in opening file\n");
    return 1;
  }

  fwrite(message,sizeof(char),strlen(message),fp);

  fclose(fp);


  fp = fopen(filename,"r");

  if(fp == NULL)
  {
    printf("Error occured  in closing file\n");
    return 1;
  }

  char buffer[100];
  char data = fread(buffer,sizeof(char),sizeof(buffer)-1,fp);
  buffer[data] = '\0';


  printf("File Contents:\n");
  printf("%s\n",buffer);


  fclose(fp);

}
