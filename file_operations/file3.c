#include <fcntl.h> //open
#include <unistd.h> //write,read,close
#include <string.h> //strlen
#include <stdio.h>   //perror
        


int main(int argc,char *argv[])
{
  if(argc != 3)
  {
    write(STDERR_FILENO, "Usage: ./fileops  <filename> <message>\n",39);
    return 1;
  }

  const char *filename = argv[1];
  const char *message  = argv[2];


  //step 1:open file for writing
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,0644);

  if(fd == -1)
  {
    perror("Error opening file for writing");
    return 1;
  }

  //step 2: write message to file
  if(write(fd,message,strlen(message))== -1)
  {
    perror("Error writing to file");
    close(fd);
    return 1;
    
  }
  close(fd); //Step 3:close file
  
  //Step 4:Open file for reading
  fd = open(filename,O_RDONLY);
  if(fd == -1)
  {
    perror("Error opening file for reading");
    return 1;
  }
  //Step 5:Read and display content
  char buffer[100];
  size_t bytesRead = read(fd,buffer,sizeof(buffer)-1);
  if(bytesRead == -1)
  {
    perror("Error reading from file");
    close(fd);
    return 1;

  }

  buffer[bytesRead] = '\0'; //Null-terminate buffer
  write(STDOUT_FILENO,"File content:\n",14);
  write(STDOUT_FILENO,buffer,bytesRead);

  close(fd);//Step 6: Close file
          
  return 0;


}
  
