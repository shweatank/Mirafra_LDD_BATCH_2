#include <stdio.h>                                                                                                                                                          
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
     pid_t pid = fork(); // Create a new process
  
     //printf("Before fork: Parent PID = %d\n", getpid());
 
     if(pid < 0)
     {
          // Fork failed
          perror("fork");
          exit(EXIT_FAILURE);
      }
      else if(pid == 0)
      {
          // Child process
         //printf("Child process started (PID: %d)\n", getpid());
         printf("Child process (PID:%d) is running 'ls -l'\n", getpid());
  
  
          execlp("ls", "ls", "-l", (char *)NULL);
  
         // If exec fails, this will be executed
         perror("execlp");
          exit(EXIT_FAILURE);
     }
      else
      {
          // Parent process
          //printf("Parent process (PID: %d) created a child (PID: %d)\n", getpid(), pid);
          printf("Parent process (PID: %d) is waiting for child...\n", getpid());
  
          wait(NULL); // Wait for child to finish
  
          printf("Child process finished. Parent (PID: %d) exiting.\n", getpid());
      }
  
     return 0;

