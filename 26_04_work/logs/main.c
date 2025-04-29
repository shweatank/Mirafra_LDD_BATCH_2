// main.c
#include <stdio.h>
#include "log.h"

int main() {
    // Open a log file
    FILE *file = fopen("log.txt", "a");
    if (!file) {
        perror("Failed to open log file");
        return 1;
    }

    // Set the log file and log level
    set_log_file(file);
    set_log_level(LOG_DEBUG);  // Set the minimum log level to debug

    // Example log messages
    log_message(LOG_DEBUG, "This is a debug message");
    log_message(LOG_INFO, "This is an info message");
    log_message(LOG_WARNING, "This is a warning message");
    log_message(LOG_ERROR, "This is an error message");
    log_message(LOG_EMERG, "This is an emergency message");
    log_message(LOG_ALERT, "This is an ALert message");
    log_message(LOG_CRIT, "This is an Critical message");
    log_message(LOG_NOTICE, "This is an Notice message");

    // Clean up and close the file
    fclose(file);

    return 0;
}
