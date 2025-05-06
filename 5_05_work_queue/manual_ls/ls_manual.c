#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 10

int main() {
    char input[128];
    char *args[MAX_ARGS];
    char cwd[100];

    while (1) {
	getcwd(cwd,sizeof(cwd));
        printf("Mohan@Mirafra:%s$ ",cwd);
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0; 

        if (strcmp(input, "exit") == 0)
            break;

        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (args[0] == NULL) continue; 

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execvp(args[0], args);
            perror("execvp failed");
            exit(1);
        } else if (pid > 0) {
            // Parent process
            wait(NULL);
        } else {
            perror("fork failed");
        }
    }

    return 0;
}

