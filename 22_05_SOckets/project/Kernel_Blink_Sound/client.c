#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "192.168.0.168" // Raspberry Pi's IP
#define PORT 12345
#define MAX_MSG_LEN 32

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[MAX_MSG_LEN];
    
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }
    
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }
    
    // Send commands (example: "blink" or "sound")
    while (1) {
        printf("Enter command (blink/sound/exit): ");
        fgets(message, MAX_MSG_LEN, stdin);
        message[strcspn(message, "\n")] = 0; // Remove newline
        
        if (strcmp(message, "exit") == 0) break;
        
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Send failed");
            break;
        }
        printf("Sent: %s\n", message);
        sleep(1); // Wait before next command
    }
    
    // Clean up
    close(sock);
    return 0;
}
