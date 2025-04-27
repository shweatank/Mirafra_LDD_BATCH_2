// log.h
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

// Log levels
#define LOG_DEBUG   0
#define LOG_INFO    1
#define LOG_WARNING 2
#define LOG_ERROR   3

// Default log level
#define LOG_LEVEL   LOG_DEBUG

// Function prototypes
void log_message(int level, const char *format, ...);
void set_log_level(int level);
void set_log_file(FILE *file);

#endif