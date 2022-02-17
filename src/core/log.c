#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>

const char* MSGHIST_FNAME = "msghist.log";
const char* DEBUGLOG_FNAME = "debug.log";

FILE* msghist_file;
FILE* debug_file;

static int indent_level = 0;

/**
 * Opens and overwrites all the logging files.
 *
 * This should be called only once at startup.
 */
void init_logs(void)
{
    msghist_file = fopen(MSGHIST_FNAME, "w");
    debug_file = fopen(DEBUGLOG_FNAME, "w");
}

/**
 * Write a message to the given log file.
 */
void log_msg(int logtype, const char* msg)
{
    switch(logtype)
    {
        case LOG_MSGHIST:
        {
            for(int i = 0; i < indent_level; ++i)
            {
                fprintf(msghist_file, "\t");
            }

            fprintf(msghist_file, "%s\n", msg);
            fflush(msghist_file);
            break;
        }
        case LOG_DEBUG:
        {
            for(int i = 0; i < indent_level; ++i)
            {
                fprintf(debug_file, "\t");
            }

            fprintf(debug_file, "%s\n", msg);
            fflush(debug_file);
            break;
        }
    }
}

/**
 * Write a formatted message with variable args to the given log file.
 */
void log_format_msg(int logtype, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char msg[256];
    vsnprintf(msg, 256, format, args);
    log_msg(logtype, msg);

    va_end(args);
}

/**
 *
 */
void log_scheck_fail(const char* msg)
{
    log_format_msg(LOG_DEBUG, "[SANITY CHECK FAILURE] %s", msg);
}

/**
 * Closes all opened log files.
 *
 * This should only be called once at shutdown
 */
void uninit_logs(void)
{
    fclose(msghist_file);
    fclose(debug_file);
}

void log_push_indent(void)
{
    ++indent_level;
}

void log_pop_indent(void)
{
    if(indent_level > 0)
    {
        --indent_level;
    }
}
