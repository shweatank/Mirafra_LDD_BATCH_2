// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>

// int main(){
// pid_t pid = fork();
// if(pid < 0){
// 	perror("fork");
// 	exit(EXIT_FAILURE);
// }else if (pid == 0){
// 	printf("child process (PID: %d) is running 'ls -l '\n",getpid());
// 	execlp("ls", "ls", "-l", (char *)NULL);

// 	perror("execlp");
// 	exit(EXIT_FAILURE);
// }else{
// 	printf("parent process (PID: %d) is waiting for child \n", getpid());
// 	wait(NULL);
// 	printf("child process finished.parents eiting\n");
//     }
// }





#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_SIZE 100

int main() {
    int fd1, fd2;
    char buffer[MAX_SIZE];
    ssize_t bytes;
    int pipefd[2];

    printf("main process PID: %d\n", getpid());

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {

        close(pipefd[1]);

        int num1, num2;
        read(pipefd[0], &num1, sizeof(int));
        read(pipefd[0], &num2, sizeof(int));
        close(pipefd[0]);

        printf("child process PID: %d, PPID: %d\n", getpid(), getppid());
        printf("child process received: %d and %d\n", num1, num2);

        int sum = num1 + num2;
        int diff = num1 - num2;

        int fd_out = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0) {
            printf("can't open output.txt: %s\n", strerror(errno));
            return 1;
        }

        snprintf(buffer, MAX_SIZE, "input: %d, %d\nsum: %d\ndiff: %d\n", num1, num2, sum, diff);
        write(fd_out, buffer, strlen(buffer));
        close(fd_out);

        printf("child wrote result to output.txt\n");
        execlp("ls", "ls", "-l", (char *)NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);

    } else {
        close(pipefd[0]);

        fd1 = open("data1.txt", O_RDONLY);
        if (fd1 < 0) {
            printf("can't open data1.txt: %s\n", strerror(errno));
            return 1;
        }
        bytes = read(fd1, buffer, MAX_SIZE - 1);
        buffer[bytes] = '\0';
        int num1 = atoi(buffer);
        close(fd1);

        fd2 = open("data2.txt", O_RDONLY);
        if (fd2 < 0) {
            printf("can't open data2.txt: %s\n", strerror(errno));
            return 1;
        }
        bytes = read(fd2, buffer, MAX_SIZE - 1);
        buffer[bytes] = '\0';
        int num2 = atoi(buffer);
        close(fd2);

        write(pipefd[1], &num1, sizeof(int));
        write(pipefd[1], &num2, sizeof(int));
        close(pipefd[1]);

        printf("parent sent %d and %d to child\n", num1, num2);
        wait(NULL);
        printf("child process has finished. Parent exiting.\n");
    }

    return 0;
}
