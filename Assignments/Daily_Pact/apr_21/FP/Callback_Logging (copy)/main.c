#include <stdio.h>

void log_console(const char *level, const char *message) {
    printf("[%s] %s\n", level, message);
}

void log_file(const char *level, const char *message) {
    FILE *file = fopen("log.txt", "a");
    fprintf(file, "[%s] %s\n", level, message);
    fclose(file);
}

void log_network(const char *level, const char *message) {
    printf("network log: [%s] %s\n", level, message);
}

int main() {
    void (*loggers[])(const char*, const char*) = {log_console, log_file, log_network};
    const char *level = "INFO";
    const char *message = "system initialized.";

    for (int i = 0; i < 3; ++i) {
        loggers[i](level, message);
    }

    return 0;
}

