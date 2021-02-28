//
// Created by ErMo on 2021/2/28.
//

#include "log.h"

void LOG_MSG(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log::log_info(LogLevel::MSG, fmt, ap);
    va_end(ap);
}

void LOG_WARN(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log::log_info(LogLevel::WARNING, fmt, ap);
    va_end(ap);
}

void LOG_ERROR(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log::log_info(LogLevel::ERROR, fmt, ap);
    va_end(ap);
}