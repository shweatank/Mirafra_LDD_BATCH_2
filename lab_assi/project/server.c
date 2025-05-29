#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_socket, client_socket;
    char buffer[1024];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char response[1024];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket error");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen
    if (listen(server_socket, 5) == 0)
        printf("Server listening on port 12345...\n");
    else {
        perror("Listen error");
        exit(1);
    }

    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    if (client_socket < 0) {
        perror("Accept failed");
        exit(1);
    }

    // Handle client requests
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected or error.\n");
            break;
        }

        // Trim newline (if fgets used on client)
	buffer[strcspn(buffer, "\n")] = '\0';

	// Debug print
	printf("Received raw command: '%s'\n", buffer);

	// Check command
	if (strcmp(buffer, "1") == 0) {
   	 strcpy(response, "Camera opened.");
	} else if (strcmp(buffer, "2") == 0) {
    	strcpy(response, "Camera closed.");
	} else if (strcmp(buffer, "3") == 0) {
	strcpy(response, "Audio playing.");
	} else if (strcmp(buffer, "4") == 0) {
   	 strcpy(response, "Audio stopped.");
	} else {
   	 strcpy(response, "Invalid command.");
	}


        // Send response to client
        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
    close(server_socket);
    return 0;
}

