#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.0.140"
#define PORT 8080
#define BUFFER_SIZE 17

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char msg1[BUFFER_SIZE] = {0};
    char msg2[BUFFER_SIZE] = {0};
    char buffer[1024] = {0};

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
	perror("socket creation failed");
	return 1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
	perror("invalid address");
	close(sock);
	return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	perror("connection failed");
	close(sock);
	exit(0);
    }
    while (1) {

	// Get user input
	printf("Enter Line1 for LCD (max 16 chars): ");
	scanf(" %[^\n]", msg1);
	printf("Enter Line2 for LCD (max 16 chars): ");
	scanf(" %[^\n]", msg2);

	// Truncate to 16 characters to fit LCD
	msg1[16] = '\0';
	msg2[16] = '\0';

	// Send messages to server
	if (send(sock, msg1, strlen(msg1) + 1, 0) < 0) {
	    perror("send msg1 failed");
	    close(sock);
	    continue;
	}
	if (send(sock, msg2, strlen(msg2) + 1, 0) < 0) {
	    perror("send msg2 failed");
	    close(sock);
	    continue;
	}

	// Receive acknowledgment
	if (recv(sock, buffer, sizeof(buffer) - 1, 0) > 0) {
	    printf("Server: %s", buffer);
	}

    }
    close(sock);

    return 0;
}
