#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#define PORT 12345
#define BUF_SIZE 4096

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr = {0};
    socklen_t addrlen = sizeof(addr);
    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");

    client_fd = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected. Receiving file...\n");

    // Open file to write the received mp4
    int file_fd = open("received_video.mp4", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd < 0) {
        perror("open");
        close(client_fd);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Receive data in chunks and write to file
    while ((bytes_read = recv(client_fd, buffer, BUF_SIZE, 0)) > 0) {
        write(file_fd, buffer, bytes_read);
    }

    if (bytes_read < 0) perror("recv");

    printf("File received successfully.\n");

    close(file_fd);
    close(client_fd);
    close(server_fd);

    return 0;
}


