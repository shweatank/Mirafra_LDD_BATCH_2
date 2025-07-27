#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(fd[1]);
        char read_fd_str[10];
        snprintf(read_fd_str, sizeof(read_fd_str), "%d", fd[0]);
        execlv("./child", (char *[]){"./child", read_fd_str, NULL});

        perror("execlv");
        exit(EXIT_FAILURE);
    } else {
        close(fd[0]);
        char message[] = "Hello from parent via pipe!\n";
        write(fd[1], message, strlen(message));
        close(fd[1]);
        wait(NULL);
    }

    return 0;
}

