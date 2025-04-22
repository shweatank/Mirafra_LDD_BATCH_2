#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[20];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        close(pipefd[1]);          // Close write end
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);
        close(pipefd[0]);
    } else {
        // Parent process
        close(pipefd[0]);          // Close read end
        strcpy(buffer, "Hello from parent!");
        write(pipefd[1], buffer, strlen(buffer) + 1);
        close(pipefd[1]);
    }

    return 0;
}
