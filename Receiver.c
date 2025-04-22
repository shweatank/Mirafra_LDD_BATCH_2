#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

int main(){
pid_t pid;
printf("enter pid to send SIGUSR1: ");
scanf("%d", &pid);

kill(pid, SIGUSR1);
printf("Sent SIGUSR1 to PID %d \n", pid);
return 0;
}


//receiver.c

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

void handle_sigusr1(int sig){

printf("received SIGUSR1! Signal number: %d\n", sig);
}

int main(){
signal(SIGUSR1, handle_sigusr1);
printf("receiver PID: %d\n, getpaid());
while(1){
pause(); //wait for signal
}
}
