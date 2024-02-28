#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>

void logging_init(const char *name, int log_level);
void logging_cleanup();
void log_message(int priority, const char *format, ...);

#define log_error(...) log_message(LOG_ERR, __VA_ARGS__)
#define log_warning(...) log_message(LOG_WARNING, __VA_ARGS__)
#define log_info(...) log_message(LOG_INFO, __VA_ARGS__)
#define log_notice(...) log_message(LOG_NOTICE, __VA_ARGS__)
#define log_debug(...) log_message(LOG_DEBUG, __VA_ARGS__)

#endif