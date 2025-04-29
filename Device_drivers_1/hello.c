#include <stdio.h>
#include "log.h"

int main() {
    // Set log level to INFO (log only INFO and above)

    // Set log file to "program.log"
    FILE *logfile = fopen("program.txt", "a");
    if (!logfile) {
        printf("Error opening log file.\n");
        return 1;
    }
    set_log_file(logfile);
    set_log_level(LOG_INFO);
    // Log messages with different levels
    log_message(LOG_INFO, "Hello, World! This is an info message.");
    log_message(LOG_WARNING, "This is a warning message.");
    log_message(LOG_ERROR, "This is an error message.");

    // Close the log file
    fclose(logfile);

    return 0;
}
