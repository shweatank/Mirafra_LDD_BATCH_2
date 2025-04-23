#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/msg.h>

struct msg_buf
{
	long msg_type;
	char msg_text[100];
};
int main()
{
	int msgid;
	key_t key=ftok("msgfile",60);
	msgid=msgget(key,0666);
	struct msg_buf msg;
	msg.msg_type=1;
	msgrcv(msgid,&msg,sizeof(msg.msg_text),1,0);
	printf("the message received is %s\n",msg.msg_text);
	msgctl(msgid,IPC_RMID,NULL);
}
