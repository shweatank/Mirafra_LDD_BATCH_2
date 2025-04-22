#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX 100

struct msg_buffer
{
    long msg_type;
    char msg_text[MAX];
};

int main()
{
    key_t key = ftok("msgfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid == -1)
    {
        perror("msgget failed");
        exit(1);
    }
    struct msg_buffer message;
    message.msg_type = 1;

    printf("Enter message : ");
    fgets(message.msg_text, MAX, stdin);

    msgsnd(msgid, &message, sizeof(message.msg_text), 0);
    printf("Message send: %s\n", message.msg_text);
}