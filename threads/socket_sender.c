#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8089

int main()
{
  int server_fd ,new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  const char *response = "Hello from server!";


  // 1.Create socket
  server_fd = socket(AE_INET,SOCK_STREAM,0);
  if(server_fd == 0)
  {
    perror("Socket failed");
    exit(EXIT_FAILURE);

  }

  //2.Bind Socket to port
  address.sin_family = AF_NET;
  address.sin_addr.s_addr = INADDR_ANY; //Any IP
  address.sin_port = htons(PORT);

  if(bind(server_fd,(struct sockaddr *)&address,sizeof(address)) < 0)
  {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  //3.Listen for incoming connections
  if(listen(server_fd,3)<0)
  {
    perror("Listen failed");
    exit(EXIT_FAILURE);

  }

  printf("Server listening on port %d ..\n",PORT);

  //4.Accept a Connection
  new_socket = accept(server_fd,(struct sockaddr *)&address,(socklen_t *)&addrlen);
  if(new_socket <0)
  {
    perror("accept failed");
    exit(EXIT_FAILURE);
  }
   
  //5.Read message from client
  read(new_socket,buffer,1024);
  printf("Received from client :%s\n",buffer);


  //6.Send response to client
  send(new_socket,response,strlen(response),0);
  printf("Response sent to client\n");

  close(new_socket);
  close(server_fd);

  return 0;


}
