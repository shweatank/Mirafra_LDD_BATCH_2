#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		write(STDERR_FILENO."usage: .fileops <filename> <message>\n",39);
		return 1;
	}
	const char *filename = argv[1];
	const char *message = argv[2];

	//step 1: Open file for writing
	int fd= open(filename), O_WRONLY | O_CREAT | 0_TRUNC, 0644);
       if(fd==-1)
       {
	       perror("Error opening file  for writing");
	       return 1;
       }

       //step 2: write message to file
       if(write(fd,message,strlen(message))==1)
       {
	       perror("Enter writing to file");
	       close(fd);
	       return 1;
       }
       close(fd); //step 3: close file

       //step 4:Open file for reading 
       fd=open(filename,O_RDONLY);
       if(fd == -1)
       {
	       perror(""Error opeming file for reading");
	       return 1;
       }
}

