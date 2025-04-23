#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<arpa/inet.h>
#include<string.h>
#define port 8080

int main()
{
	int server_fd,new_socket;
	struct sockaddr_in address;
	char *msg="hello,from server!!!";
	char buffer[1024]={0};
	socklen_t addr_len=sizeof(address);
	server_fd=socket(AF_INET,SOCK_STREAM,0);
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=INADDR_ANY;
	address.sin_port=htons(port);
	if(bind(server_fd,(struct sockaddr *)&address,sizeof(address))<0)
	{
		printf("error");
		exit(0);
	}
	if(listen(server_fd,3)<0)
	{
		printf("error");
		exit(0);
	}
	new_socket=accept(server_fd,(struct sockaddr *)&address,&addr_len);

	read(new_socket,buffer,1024);
	printf("data:%s\n",buffer);

	send(new_socket,msg,strlen(msg),0);
	printf("msg send to client\n");
	close(new_socket);
	close(server_fd);
}
