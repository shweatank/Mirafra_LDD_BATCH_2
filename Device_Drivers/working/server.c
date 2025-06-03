//tcp_server.c
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEVICE "/dev/lcd_ioctl"
#define LCD_IOCTL_MAGIC 'M'
#define LCD_IOCTL_LINE1 _IOW(LCD_IOCTL_MAGIC, 1, char *)
#define LCD_IOCTL_LINE2 _IOW(LCD_IOCTL_MAGIC, 2, char *)
#define PORT 8080
#define BUFFER_SIZE 17

int main() {
    int server_fd, new_socket, fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char msg1[BUFFER_SIZE] = {0};
    char msg2[BUFFER_SIZE] = {0};

    // Open LCD device
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
	perror("open LCD device");
	return 1;
    }

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
	perror("socket creation failed");
	close(fd);
	return 1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
	perror("setsockopt");
	close(fd);
	close(server_fd);
	return 1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	perror("bind failed");
	close(fd);
	close(server_fd);
	return 1;
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
	perror("listen failed");
	close(fd);
	close(server_fd);
	return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (new_socket < 0) {
	perror("accept failed");
    }

    printf("Client connected\n");

    while (1) {

	// Receive messages
	if (recv(new_socket, msg1, BUFFER_SIZE - 1, 0) <= 0) {
	    perror("recv msg1");
	    close(new_socket);
	    exit(0);
	}
	if (recv(new_socket, msg2, BUFFER_SIZE - 1, 0) <= 0) {
	    perror("recv msg2");
	    close(new_socket);
	    exit(0);
	}

	// Ensure null termination
	msg1[BUFFER_SIZE - 1] = '\0';
	msg2[BUFFER_SIZE - 1] = '\0';

	printf("Received Line1: %s\n", msg1);
	printf("Received Line2: %s\n", msg2);

	// Write to LCD using IOCTL
	if (ioctl(fd, LCD_IOCTL_LINE1, msg1) < 0) {
	    perror("ioctl LINE1");
	}
	if (ioctl(fd, LCD_IOCTL_LINE2, msg2) < 0) {
	    perror("ioctl LINE2");
	}

	// Send acknowledgment to client
	const char *ack = "Messages received and displayed\n";
	send(new_socket, ack, strlen(ack), 0);

    }
    close(new_socket);

    close(fd);
    close(server_fd);
    return 0;
}
