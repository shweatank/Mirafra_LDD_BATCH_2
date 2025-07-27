#include <stdio.h>                                                                                                                                                          
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


void parentFunction()
{
  printf("Parent (PID:%d):Running parentFunction()\n",getpid());
  for(int i =0;i<5;i++)
  {
    printf("Parent is working..(%d)\n",i+1);
    sleep(1);
  }

  printf("Parent finished work.\n");
}


int main()
{

  pid_t pid = fork();

  if(pid <0)
  {
    perror("fork failed");
    exit(EXIT_FAILURE);

  }
  else if(pid == 0)
  {
    //In child process
    printf("Child (PID:%d):Executing 'echo'\n",getpid());
    //You can replace "echo" with any executable or even your own program
    execlp("echo","echo","Child says hello from exec!",(char *)NULL);

    //of exec fails
    perror("execlp failed");
    exit(EXIT_FAILURE);
  }
   else
   {
     //In parent process
     parentFunction();

     //wait for child to finish
     wait(NULL);
     printf("Parent : Child process has finished.\n");

   }
   return 0;
}
