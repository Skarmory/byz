#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>

const char* MSGHIST_FNAME = "msghist.log";
const char* DEBUGLOG_FNAME = "debug.log";
const char* TESTLOG_FNAME = "test.log";

FILE* msghist_file;
FILE* debug_file;
FILE* test_file;

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
    test_file = fopen(TESTLOG_FNAME, "w");
}

static void _log_msg(FILE* file, const char* msg)
{
    for(int i = 0; i < indent_level; ++i)
    {
        fprintf(file, "\t");
    }

    fprintf(file, "%s\n", msg);
    fflush(file);
}

/**
 * Write a message to the given log file.
 */
void log_msg(LogChannels channels, const char* msg)
{
    if(bit_flags_has_flags(channels, LOG_MSGHIST))
    {
        _log_msg(msghist_file, msg);
    }

    if(bit_flags_has_flags(channels, LOG_DEBUG))
    {
        _log_msg(debug_file, msg);
    }

    if(bit_flags_has_flags(channels, LOG_TEST))
    {
        _log_msg(test_file, msg);
    }

    if(bit_flags_has_flags(channels, LOG_STDOUT))
    {
        _log_msg(stdout, msg);
    }
}

/**
 * Write a formatted message with variable args to the given log file.
 */
void log_format_msg(LogChannels channels, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char msg[256];
    vsnprintf(msg, 256, format, args);
    log_msg(channels, msg);

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
    fclose(test_file);
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
