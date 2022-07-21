#ifndef BYZ_CORE_LOG_H
#define BYZ_CORE_LOG_H

#include "core/bit_flags.h"

enum LogChannel
{
    LOG_MSGHIST = BIT_FLAG(0),
    LOG_DEBUG   = BIT_FLAG(1),
    LOG_TEST    = BIT_FLAG(2),
    LOG_STDOUT  = BIT_FLAG(3)
};
typedef unsigned int LogChannels;

void init_logs(void);
void uninit_logs(void);
void log_msg(LogChannels channels, const char* msg);
void log_format_msg(LogChannels channels, const char* format, ...);
void log_scheck_fail(const char* msg);
void log_push_indent(void);
void log_pop_indent(void);

#endif
