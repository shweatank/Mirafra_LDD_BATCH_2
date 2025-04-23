#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
    pid_t pid=fork();
    if(pid<0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid==0)
    {
        printf("Child process (Pid: %d) is running 'ls -l'\n",getpid());
        execlp("ls","ls","-l",(char*)NULL);
        
        //if exe fails
        perror("eexeclp");
        exit(EXIT_FAILURE);
    }
    else
    {
        //parent process
        printf("parent process (Pid: %d) is running 'ls -l'\n",getpid());
        wait(NULL);
        printf("Child process finished parent exiting\n");
        
        
    }
}
