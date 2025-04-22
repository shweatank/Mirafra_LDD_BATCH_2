#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX 100

struct msg_buffer
{
    long msg_type;
    char msg_text[MAX];
};

int main(){
    key_t key = ftok("msgfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid == -1){
        perror("msgget failed");
        exit(1);
    }
    struct msg_buffer message;

    msgsnd(msgid, &message, sizeof(message.msg_text),0);
    printf("Message read: %s\n",message.msg_text);
    msgctl(msgid, IPC_RMID,NULL);
    return 0;
}