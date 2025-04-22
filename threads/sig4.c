#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main()
{
    pid_t pid1,pid2;
    printf("Enter PID1 to send SIGUSR1: ");
    scanf("%d", &pid1);
    

    if (kill(pid1, SIGUSR1) == 0) 
    {
        printf("Sent SIGUSR1 to PID1 %d\n", pid1);
    } else 
    {
        perror("Failed to send signal");
    }

      printf("Enter PID2 to send SIGUSR2: ");
       scanf("%d", &pid2);
  
  
      if (kill(pid2, SIGUSR2) == 0)
       {
           printf("Sent SIGUSR2 to PID2 %d\n", pid2);
       } else
       {
           perror("Failed to send signal");
     }




  
    


    return 0;
}


