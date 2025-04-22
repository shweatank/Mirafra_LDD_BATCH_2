#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int sig){
printf("Handled signal %d using sigaction \n", sig);
}

int main(){
struct sigaction sa;
sa.sa_handler=sig_handler;
sigemptyset(&sa.sa_mask);

sigaction(SIGINT, &sa, NULL);
while(1{
printf("Waiting for SIGNIT....\n");
sleep(2);
}
}


//block_signal.c
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int sig){
printf("signal %d received \n", sig);
}

int main(){
signal(SIGINT, handler);

sigset_t blockset;
sigemptyset(&block_set);
sigaddset(&block_set, SIGINT);

printf("Blocking SIGINT for 5 seconds...\n");
sigprocmask(SIG_UNBLOCK, &block_set, NULL);

while(1){
pause()
}
}
