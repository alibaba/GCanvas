/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_LOG_H
#define GCANVAS_LOG_H

#if defined(__ANDROID__)
#include <android/log.h>
#endif
#include <iostream>
#include <vector>

#include "export.h"




//namespace gcanvas
//{

    typedef void (*GCanvasSystemLog)(const char *tag, const char *log);

API_EXPORT extern GCanvasSystemLog gcanvasSystemLog;


typedef enum {
        LOG_LEVEL_DEBUG = 0,
        LOG_LEVEL_INFO,
        LOG_LEVEL_WARN,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_FATAL
    } LogLevel;


    struct GCanvasHooks
    {
        void (*GCanvasException)(const char *canvasId, const char *tag, const char *detail);
    };


    struct GCanvasLog
    {
        GCanvasLog();
        std::string tag;
        std::string detail;
    };


    void fillLogInfo(GCanvasLog &log, const char *tag, const char *format, ...);


    void AppendErrorLogInfo(std::vector<GCanvasLog> *errVec, const char *tag, const char *format, ...);


#if defined(__ANDROID__)
    int TransLogLevel(LogLevel logLevel);
    #endif

    void LogExt(LogLevel logLevel, const char *tag, const char *format, ...);
    void SetLogLevel(LogLevel logLevel);
    LogLevel GetLogLevel();


    API_EXPORT void LogException(GCanvasHooks *hooks, std::string contextId, const char *tag, const char *format, ...);


    API_EXPORT void LogExceptionVector(GCanvasHooks *hooks, std::string contextId, std::vector<GCanvasLog> &vec);

    //#ifdef IOS
    //#define DISABLE_LOG
    //#endif

    #ifdef DISABLE_LOG
    #define LOG_D(...) ;
    #define LOG_I(...) ;
    #define LOG_W(...) ;
    #define LOG_E(...) ;
    #define LOG_F(...) ;
    #define LOG_EXT(...) ;
    #define LOG_EXCEPTION(hooks, contextId, tag, ...);
    #define LOG_EXCEPTION_VECTOR(hooks, contextId, std::vector<GCanvasLog> *vec);
    #else
    #define LOG_TAG_NAME "gcanvasCore"
    #define LOG_D(...) LogExt(LOG_LEVEL_DEBUG, LOG_TAG_NAME, __VA_ARGS__)
    #define LOG_I(...) LogExt(LOG_LEVEL_INFO, LOG_TAG_NAME, __VA_ARGS__)
    #define LOG_W(...) LogExt(LOG_LEVEL_WARN, LOG_TAG_NAME, __VA_ARGS__)
    #define LOG_E(...) LogExt(LOG_LEVEL_ERROR, LOG_TAG_NAME, __VA_ARGS__)
    #define LOG_F(...) LogExt(LOG_LEVEL_FATAL, LOG_TAG_NAME, __VA_ARGS__)
    #define LOG_EXT LogExt
    #define LOG_EXCEPTION(hooks, contextId, tag, ...) LogException(hooks, contextId, tag, __VA_ARGS__)
    #define LOG_EXCEPTION_VECTOR(hooks, contextId, vec) LogExceptionVector(hooks, contextId, vec)
    #endif
//}

#endif /* GCANVAS_LOG_H */
