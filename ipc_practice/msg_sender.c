#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
struct msg_buf
{
	long msg_type;
	char msg_text[100];
};

int main()
{
	int msgid;
	key_t  key=ftok("msgfile",60);
	msgid=msgget(key,0666|IPC_CREAT);
	if(msgid==-1)
	{
		printf("error");
		exit(0);
	}
	struct msg_buf msg;
	msg.msg_type=1;
	printf("enter message to send;\n");
	fgets(msg.msg_text,100,stdin);
	msgsnd(msgid,&msg,sizeof(msg.msg_text),0);
	printf("the sent message is %s\n",msg.msg_text);
}
