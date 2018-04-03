/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "Log.h"

namespace gcanvas
{
LogLevel g_log_level = LOG_LEVEL_INFO;

LogLevel GetLogLevel() { return g_log_level; };

void SetLogLevel(LogLevel logLevel)
{
    if( g_log_level == logLevel ) return;
#ifdef DEBUG
    LogLevel oldLevel = g_log_level;
    LOG_I("[SetLogLevel] %d=>%d", oldLevel, logLevel);
#endif
    g_log_level = logLevel;
};

#if defined(__ANDROID__)
int TransLogLevel(LogLevel logLevel)
{
    switch (logLevel){
        case LOG_LEVEL_VERBOSE:
            return ANDROID_LOG_VERBOSE;
        case LOG_LEVEL_DEBUG:
            return ANDROID_LOG_DEBUG;
        case LOG_LEVEL_INFO:
            return ANDROID_LOG_INFO;
        case LOG_LEVEL_WARN:
            return ANDROID_LOG_WARN;
        case LOG_LEVEL_ERROR:
            return ANDROID_LOG_ERROR;
        case LOG_LEVEL_FATAL:
            return ANDROID_LOG_FATAL;
        default:
            return ANDROID_LOG_DEFAULT;
    }
}
#endif

void LogExt(LogLevel logLevel, const char *tag, const char *format, ...)
{
    if (g_log_level > logLevel) return;

    va_list va;
    char buffer[1024];

    va_start(va, format);
    vsnprintf(buffer, 1024, format, va);
    va_end(va);

#if defined(__ANDROID__)
    __android_log_write(TransLogLevel(logLevel), tag, buffer);
#else
    printf("LOG: %s\n", buffer);
#endif
}
}
