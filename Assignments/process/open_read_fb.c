#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
 #include <fcntl.h>

#include<unistd.h>
int main()
{
  int fp=open("file.txt",O_WRONLY );
  if(fp==-1)
  {
       printf("file to open file\n");
   }
   char buf[10];
   write(fp,"hello",5);
   close(fp);
   fp=open("file.txt",O_RDONLY);
   size_t ret=read(fp,buf,5);
   buf[ret]='\0';
   printf("%s\n",buf);
   close(fp);
   return 0;
}
   
