#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>

const char* MSGHIST_FNAME = "msghist.log";
const char* DEBUGLOG_FNAME = "debug.log";
const char* TESTLOG_FNAME = "test.log";

struct _LogChannel
{
    FILE* file;
    int indent;
};

static struct _LogChannel s_log_channels[32];

/**
 * Opens and overwrites all the logging files.
 *
 * This should be called only once at startup.
 */
void init_logs(void)
{
    s_log_channels[LOG_ID_MSGHIST].file = fopen(MSGHIST_FNAME, "w");
    s_log_channels[LOG_ID_MSGHIST].indent = 0;

    s_log_channels[LOG_ID_DEBUG].file = fopen(DEBUGLOG_FNAME, "w");
    s_log_channels[LOG_ID_DEBUG].indent = 0;

    s_log_channels[LOG_ID_TEST].file = fopen(TESTLOG_FNAME, "w");
    s_log_channels[LOG_ID_TEST].indent = 0;

    s_log_channels[LOG_ID_STDOUT].file = stdout;
    s_log_channels[LOG_ID_STDOUT].indent = 0;
}

static void _log_msg(struct _LogChannel* channel, const char* msg)
{
    for(int i = 0; i < channel->indent; ++i)
    {
        fprintf(channel->file, "\t");
    }

    fprintf(channel->file, "%s\n", msg);
    fflush(channel->file);
}

/**
 * Write a message to the given log file.
 */
void log_msg(LogChannels channels, const char* msg)
{
    if(bit_flags_has_flags(channels, LOG_MSGHIST))
    {
        _log_msg(&s_log_channels[LOG_ID_MSGHIST], msg);
    }

    if(bit_flags_has_flags(channels, LOG_DEBUG))
    {
        _log_msg(&s_log_channels[LOG_ID_DEBUG], msg);
    }

    if(bit_flags_has_flags(channels, LOG_TEST))
    {
        _log_msg(&s_log_channels[LOG_ID_TEST], msg);
    }

    if(bit_flags_has_flags(channels, LOG_STDOUT))
    {
        _log_msg(&s_log_channels[LOG_ID_STDOUT], msg);
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
    fflush(s_log_channels[LOG_ID_MSGHIST].file);
    fclose(s_log_channels[LOG_ID_MSGHIST].file);

    fflush(s_log_channels[LOG_ID_DEBUG].file);
    fclose(s_log_channels[LOG_ID_DEBUG].file);

    fflush(s_log_channels[LOG_ID_TEST].file);
    fclose(s_log_channels[LOG_ID_TEST].file);
}

void log_push_indent(LogChannels channels)
{
    if(bit_flags_has_flags(channels, LOG_MSGHIST))
    {
        ++s_log_channels[LOG_ID_MSGHIST].indent;
    }

    if(bit_flags_has_flags(channels, LOG_DEBUG))
    {
        ++s_log_channels[LOG_ID_DEBUG].indent;
    }

    if(bit_flags_has_flags(channels, LOG_TEST))
    {
        ++s_log_channels[LOG_ID_TEST].indent;
    }

    if(bit_flags_has_flags(channels, LOG_STDOUT))
    {
        ++s_log_channels[LOG_ID_STDOUT].indent;
    }
}

void log_pop_indent(LogChannels channels)
{
    if(bit_flags_has_flags(channels, LOG_MSGHIST))
    {
        if(s_log_channels[LOG_ID_MSGHIST].indent > 0)
        {
            --s_log_channels[LOG_ID_MSGHIST].indent;
        }
    }

    if(bit_flags_has_flags(channels, LOG_DEBUG))
    {
        if(s_log_channels[LOG_ID_DEBUG].indent > 0)
        {
            --s_log_channels[LOG_ID_DEBUG].indent;
        }
    }

    if(bit_flags_has_flags(channels, LOG_TEST))
    {
        if(s_log_channels[LOG_ID_TEST].indent > 0)
        {
            --s_log_channels[LOG_ID_TEST].indent;
        }
    }

    if(bit_flags_has_flags(channels, LOG_STDOUT))
    {
        if(s_log_channels[LOG_ID_STDOUT].indent > 0)
        {
            --s_log_channels[LOG_ID_STDOUT].indent;
        }
    }
}
