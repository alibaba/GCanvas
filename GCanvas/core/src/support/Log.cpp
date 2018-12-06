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
    if (LOG_LEVEL_DEBUG == logLevel) return ANDROID_LOG_DEBUG;
    if (LOG_LEVEL_INFO == logLevel) return ANDROID_LOG_INFO;
    if (LOG_LEVEL_WARN == logLevel) return ANDROID_LOG_WARN;
    if (LOG_LEVEL_ERROR == logLevel) return ANDROID_LOG_ERROR;
    if (LOG_LEVEL_FATAL == logLevel) return ANDROID_LOG_FATAL;
    return ANDROID_LOG_DEFAULT;
}
#endif

void LogExt(LogLevel logLevel, const char *tag, const char *format, ...)
{
//    if (g_log_level > logLevel) return;

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
