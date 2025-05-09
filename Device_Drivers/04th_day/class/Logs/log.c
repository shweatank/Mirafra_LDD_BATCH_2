// log.c
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Default log file pointer
static FILE *log_file = NULL;

// Current log level
static int current_log_level = LOG_LEVEL;

// Helper function to get current time as string
static const char *get_time_string() {
    static char buffer[20];
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

// Helper function to print log level as a string
static const char *log_level_to_string(int level) {
    switch (level) {
        case LOG_DEBUG:   return "DEBUG";
        case LOG_INFO:    return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR:   return "ERROR";
        default:          return "UNKNOWN";
    }
}

// Function to set the log level
void set_log_level(int level) {
    current_log_level = level;
}

// Function to set the log file (if you want to log to a specific file)
void set_log_file(FILE *file) {
    log_file = file;
}

// Core logging function
void log_message(int level, const char *format, ...) {
    if (level < current_log_level) {
        return; // Don't log if the level is lower than the current level
    }

    va_list args;
    va_start(args, format);

    const char *time_str = get_time_string();
    const char *level_str = log_level_to_string(level);

    // Print log to console
    printf("[%s] [%s] ", time_str, level_str);
    vprintf(format, args);
    printf("\n");

    // Print log to file (if log file is set)
    if (log_file) {
        fprintf(log_file, "[%s] [%s] ", time_str, level_str);
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
    }

    va_end(args);
}