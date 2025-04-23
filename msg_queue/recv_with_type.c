#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
       #include <sys/ipc.h>
       #include <sys/msg.h>
 struct msgbuf {
               long mtype;       /* message type, must be > 0 */
               char mtext[100];    /* message data */
           };


int main(){
key_t key=ftok("msgqueue1",65);
int id=msgget(key,0666|IPC_CREAT);
if(id<0){
perror("msgget:");
return 0;
}
struct msgbuf msg;
int n;
printf("enter the type of the msg:");
scanf("%d",&n);
int snd=msgrcv(id,&msg,sizeof(msg),n,0);
if(snd<0){

perror("msgsnd");
return 0;
}
printf("%s\n",msg.mtext);
}
