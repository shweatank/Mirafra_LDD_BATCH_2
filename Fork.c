#include<sys/wait.h>

void parentFunction(){
printf("%d \n", getpid());
for(int i=0;i<5;i++){
printf("%d \n", i+1);
sleep(1);
printf("\n");
}

int main(){
pid_t pid= fork();

if(pid < 0){

perror("fork failed");
exit(EXIT_FAILURE);
}
else if(pid == 0){

printf("%d \n", getpid());
execlp("echo","echo,"child says hello from exec!, (char *)NULL);
perror("execlp failed");
exit(EXIT_FAILURE);
}
else{
