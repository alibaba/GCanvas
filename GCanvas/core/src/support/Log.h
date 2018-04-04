/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCANVAS_LOG_H__
#define __GCANVAS_LOG_H__

#if defined(__ANDROID__)
#include <android/log.h>
#endif
#include <iostream>

namespace gcanvas
{
typedef enum {
    LOG_LEVEL_VERBOSE = -1,
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

#if defined(__ANDROID__)
int TransLogLevel(LogLevel logLevel);
#endif

void LogExt(LogLevel logLevel, const char *tag, const char *format, ...);
void SetLogLevel(LogLevel logLevel);
LogLevel GetLogLevel();

#if defined(__ANDROID__)
#ifdef NDEBUG
#define DISABLE_LOG
#endif
#endif


//#ifdef IOS
//
//#ifdef DEBUG
//#else
//#define DISABLE_LOG
//#endif
//
//#endif

#ifdef DISABLE_LOG
#define LOG_V(...) ;
#define LOG_D(...) ;
#define LOG_I(...) ;
#define LOG_W(...) ;
#define LOG_E(...) ;
#define LOG_F(...) ;
#define LOG_EXT(...) ;
#else
#define LOG_TAG_NAME "gcanvas.native"

#define LOG_V(...) gcanvas::LogExt(gcanvas::LOG_LEVEL_VERBOSE, LOG_TAG_NAME, __VA_ARGS__)
#define LOG_D(...) gcanvas::LogExt(gcanvas::LOG_LEVEL_DEBUG, LOG_TAG_NAME, __VA_ARGS__)
#define LOG_I(...) gcanvas::LogExt(gcanvas::LOG_LEVEL_INFO, LOG_TAG_NAME, __VA_ARGS__)
#define LOG_W(...) gcanvas::LogExt(gcanvas::LOG_LEVEL_WARN, LOG_TAG_NAME, __VA_ARGS__)
#define LOG_E(...) gcanvas::LogExt(gcanvas::LOG_LEVEL_ERROR, LOG_TAG_NAME, __VA_ARGS__)
#define LOG_F(...) gcanvas::LogExt(gcanvas::LOG_LEVEL_FATAL, LOG_TAG_NAME, __VA_ARGS__)
#define LOG_EXT gcanvas::LogExt
#endif
}

//class Log
//{
//public:
//    // -----------------------------------------------------------
//    // --                     Debug logging                     --
//    // -----------------------------------------------------------
//    static void DLog(const char *format, ...)
//    {
//        va_list va;
//        char buffer[1024];
//        //
//        //  format and output the message..
//        //
//        va_start(va, format);
//        vsnprintf(buffer, 1024, format, va);
//        va_end(va);
//
//        LOG_D(buffer);
//    }
//
//    // fix font can't display bug second,strong method
//    static void DLog_forFont(const char *format, ...)
//    {
//        va_list va;
//        char buffer[1024];
//        //
//        //  format and output the message..
//        //
//        va_start(va, format);
//        vsnprintf(buffer, 1024, format, va);
//        va_end(va);
//
//        LOG_I(buffer);
//    }
//};

#endif
