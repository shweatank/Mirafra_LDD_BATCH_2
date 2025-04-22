#include <stdio.h>
#include <string.h>

void start()  { puts("Started"); }
void stop()   { puts("Stopped"); }
void pause_() { puts("Paused"); }
void exit_()  { puts("Exiting"); }

struct Command {
    char *cmd;
    void (*func)();
};

int main() {
    struct Command cmds[] = {
        {"start", start},
        {"stop", stop},
        {"pause", pause_},
        {"exit", exit_}
    };

    char input[20];
    while (1) {
        printf("cmd> ");
        scanf("%s", input);

        int found = 0;
        for (int i = 0; i < 4; ++i) {
            if (strcmp(input, cmds[i].cmd) == 0) {
                cmds[i].func();
                found = 1;
                if (strcmp(input, "exit") == 0) return 0;
                break;
            }
        }
        if (!found) puts("unknown command");
    }

    return 0;
}

