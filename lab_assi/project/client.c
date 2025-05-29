#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int client_socket;
    int flag = 1;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket error");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error");
        exit(1);
    }

    while (flag) {
        printf("\nMenu:\n");
        printf("1 for open the camera\n");
        printf("2 for close the camera\n");
        printf("3 for play the audio\n");
        printf("4 for close the sound\n");
        printf("Enter the given number: ");

        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline

        // Send input to server
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive response
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);

        printf("Response from server: %s\n", buffer);

        printf("Do you want to continue? (1 = Yes / 0 = No): ");
        scanf("%d", &flag);
        getchar();  // Clear newline left in buffer
    }

    close(client_socket);
    return 0;
}

