#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<arpa/inet.h>
#include<string.h>
#define port 8080

int main()
{
	struct sockaddr_in a;
	char *msg="hello from client";
	char buf[1024]={0};
	int id=socket(AF_INET,SOCK_STREAM,0);
	a.sin_family=AF_INET;
	a.sin_port=htons(port);
	inet_pton(id,"127.0.0.1",&a.sin_addr);
	connect(id,(struct sockaddr *)&a,sizeof(a));
	send(id,msg,strlen(msg),0);
	printf("msg send to server");

	read(id,buf,sizeof(buf));
	printf("message to server:%s",buf);
	close(id);
}
