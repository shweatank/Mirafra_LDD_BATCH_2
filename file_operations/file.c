#include <stdio.h>
#include <string.h>


int main()
{
  FILE *fp;
  const char *filename = "sample.txt";
  const char *message = "Hello\n File";


  //step 1:open file for writing ("w" mode will create or overwrite the file)
  fp = fopen(filename,"w");
  if(fp == NULL)
  {
    perror("Error opening file for writing");
    return 1;
  }

  //step2:Write message to file
  fwrite(message,sizeof(char),strlen(message),fp);

  //step3: Close the file
  fclose(fp);

  //step4: Open a file again for reading("r" mode)
  fp = fopen(filename ,"r");

  if(fp == NULL)
  {
    perror("Error opening file for reading");
    return 1;
  }

  //Step 5: Read Content and print
  char buffer[100];
  size_t bytesRead = fread(buffer,sizeof(char),sizeof(buffer)-1,fp);
  buffer[bytesRead] = '\0';

  printf("File Content \n%s\n",buffer);

  //step 6 :Close the file
  fclose(fp);

  return 0;

    
}
