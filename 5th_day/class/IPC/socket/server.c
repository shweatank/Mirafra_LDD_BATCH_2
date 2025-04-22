//server.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *response = "Hello from server!";

    // 1. create socket
    server_fd  = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0)
    {
        perror("socket failed ");
        exit(EXIT_FAILURE);
    }

    //2. Bind socket to PORT
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // ANY IP 
    address.sin_port = htons(PORT);

    if (bind(server_fd,(struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    //3, Listen for incoming connection
        if(listen(server_fd, 3) < 0)
        {
            perror("Listen failed");
            exit(EXIT_FAILURE); 
        }
        printf("Server is listening on port %d\n", PORT);

        // 4. Accept a connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if(new_socket < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE); 
        }

        // 5. Read a message from client
        read(new_socket, buffer, 1024);
        printf("Response sent to client \n");

        // 6. Send Response to client
        send(new_socket, response, strlen(response), 0);
        printf("Response sent to client\n");

        close(new_socket);
        close(server_fd);
        return 0;
    
}