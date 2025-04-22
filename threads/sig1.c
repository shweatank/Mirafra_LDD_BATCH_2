#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_signint(int sig)
{
  printf("\nCaught signal %d (SIGNIT). Existing gracefully.\n",sig);
  //exit(0)  
}

int main()
{
  //signal(SIGINT,handle_signint); //Catch Ctrl+C
                                

  while(1)
  {
    printf("Running....Press Ctrl+C to stop.\n");
    sleep(1);
  }
}
