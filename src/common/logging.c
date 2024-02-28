#include "common/logging.h"

void logging_init(const char *name, int log_level)
{
    setlogmask(LOG_UPTO(log_level));

    openlog(name, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
}

void logging_cleanup()
{
    closelog();
}

void log_message(int priority, const char *format, ...)
{

    va_list args;
    va_start(args, format);

    // Send the log message to syslog
    vsyslog(priority, format, args);

    // Also print the log message to the console
    vprintf(format, args);
    printf("\n");

    va_end(args);
}