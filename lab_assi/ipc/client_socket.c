#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/ipc.h>
#include<netinet/in.h>
#define PORT 8080 //http port no

int main(){
	int sock=0;
	struct sockaddr_in serv_addr;
	char buffer[1024]={0};
	const char *response="hello from client\n";

	//1.create socket
	server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(server_fd==0){
		perror("socket failed\n");
		exit(1);
	}

	//4.accept a connection
	new_socket=accept(server_fd,(struct sockaddr *)&address,(socklen_t *)&addrlen);
	if(new_socket < 0){
		perror("accept failed\n");
		exit(1);
	}

	//6.send response to client
	send(new_socket,response,strlen(response),0);
	printf("response sent to client\n");

	//5.read message from client
	read(new_socket,buffer,1024);
	printf("recived from client :%s\n", buffer);

	close(new_socket);
	close(server_fd);


	return 0;

}

