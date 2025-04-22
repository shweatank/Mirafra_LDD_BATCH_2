#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/ipc.h>
#include<netinet/in.h>

#define PORT 8080 //http port no

int main(){
	int server_fd,new_socket;
	struct sockaddr_in address;
	int addrlen=sizeof(address);
	char buffer[1024]={0};
	const char *response="hello from server!";

	//1.create socket
	server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(server_fd==0){
		perror("socket failed\n");
		exit(1);
	}

	//2.bind socket to port
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=INADDR_ANY;  //any IP
	address.sin_port=htons(PORT);

	if(bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0){
		perror("bind failed\n");
		exit(1);
	}

	//3.listen for incomming connection
	if(listen(server_fd,3)<0){
		perror("listen failed\n");
		exit(1);
	}
	printf("server listing on port %d..\n",PORT);

	
	//4.accept a connection
	new_socket=accept(server_fd,(struct sockaddr *)&address,(socklen_t *)&addrlen);
	if(new_socket < 0){
		perror("accept failed\n");
		exit(1);
	}


	//5.read message from client
	read(new_socket,buffer,1024);
	printf("recived from client :%s\n", buffer);


	//6.send response to client
	send(new_socket,response,strlen(response),0);
	printf("response sent to client\n");

	close(new_socket);
	close(server_fd);


	return 0;

}



