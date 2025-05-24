#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <RaspberryPi_IP> <file_path>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    const char *file_path = argv[2];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    if (inet_pton(AF_INET, server_ip, &server.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    int file_fd = open(file_path, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        close(sock);
        return 1;
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(file_fd, buffer, BUF_SIZE)) > 0) {
        ssize_t sent = 0;
        while (sent < bytes_read) {
            ssize_t n = send(sock, buffer + sent, bytes_read - sent, 0);
            if (n <= 0) {
                perror("send");
                close(file_fd);
                close(sock);
                return 1;
            }
            sent += n;
        }
    }

    if (bytes_read < 0) perror("read");

    printf("File sent successfully.\n");

    close(file_fd);
    close(sock);

    return 0;
}


