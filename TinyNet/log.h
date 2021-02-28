//
// Created by ErMo on 2021/2/27.
//

#ifndef REACTORSERVER_LOG_H
#define REACTORSERVER_LOG_H


#include "syslog.h"
#include <cstdarg>
#include <string>

enum LogLevel: int {
    DEBUG = 0,
    MSG,
    WARNING,
    ERROR
};

class log {
public:
    static void log_base(int level, const char* msg)
    {
        const char *level_str;
        switch (level) {
            case LogLevel::DEBUG:
                level_str = "DEBUG";
                break;
            case LogLevel::MSG:
                level_str = "MSG";
                break;
            case LogLevel::WARNING:
                level_str = "WARN";
                break;
            case LogLevel::ERROR:
                level_str = "ERR";
                break;
            default:
                level_str = "???";
                break;
        }
        fprintf(stdout, "[%s] %s\n", level_str, msg);
    }

    static void log_info(int level, const char *fmt, va_list ap)
    {
        char buf[1024];

        if (fmt != nullptr)
            vsnprintf(buf, sizeof(buf), fmt, ap);
        else
            buf[0] = '\0';

        log_base(level, buf);
    }
};


void LOG_MSG(const char* fmt, ...);

void LOG_WARN(const char* fmt, ...);

void LOG_ERROR(const char* fmt, ...);

#endif //REACTORSERVER_LOG_H