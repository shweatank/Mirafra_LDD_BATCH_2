//Simple Command Execution System Using Function Pointers


#include <stdio.h>
#include <string.h>

void help(char* arg) {
    printf("Available commands: help, echo, quit\n");
}

void echo(char* arg) {
    printf("Echo: %s\n", arg);
}

void quit(char* arg) {
    printf("Quitting program.\n");
}

int main() {
    struct {
        char* cmd;
        void (*handler)(char*);
    } commands[] = {
        {"help", help},
        {"echo", echo},
        {"quit", quit}
    };

    char line[256];
    char cmd[50], arg[200];
    while (1) {
        printf(">> ");
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;
        sscanf(line, "%s %[^\n]", cmd, arg);

        int found = 0;
        for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
            if (strcmp(cmd, commands[i].cmd) == 0) {
                commands[i].handler(arg);
                found = 1;
                if (strcmp(cmd, "quit") == 0) return 0;
                break;
            }
        }

        if (!found) {
            printf("Unknown command: %s\n", cmd);
        }
    }

    return 0;
}

