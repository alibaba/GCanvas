/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCVLog_h
#define GCVLog_h

#import <Foundation/Foundation.h>

#define ENABLE_GCVLOG

#ifdef ENABLE_GCVLOG
    #define GCVLOG_METHOD GCVLOG_D
    #define GCVLOG_FUNC(level, fmt, ...) [GCVLog writeLog:level funcName:__FUNCTION__ format:fmt, ##__VA_ARGS__];

    #define GCVLOG_D(fmt, ...) GCVLOG_FUNC(GCVLogLevelDebug, fmt, ##__VA_ARGS__);
    #define GCVLOG_I(fmt, ...) GCVLOG_FUNC(GCVLogLevelInfo, fmt, ##__VA_ARGS__);
    #define GCVLOG_W(fmt, ...) GCVLOG_FUNC(GCVLogLevelWarn, fmt, ##__VA_ARGS__);
    #define GCVLOG_E(fmt, ...) GCVLOG_FUNC(GCVLogLevelError, fmt, ##__VA_ARGS__);
#else
    #define GCVLOG_D(...)
    #define GCVLOG_I(...)
    #define GCVLOG_W(...)
    #define GCVLOG_E(...)
#endif


typedef NS_ENUM(NSInteger, GCVLogLevel) {
    GCVLogLevelDebug = 0,
    GCVLogLevelInfo,
    GCVLogLevelWarn,
    GCVLogLevelError
};

@interface GCVLog : NSObject

@property(nonatomic, assign) GCVLogLevel logLevel;

+ (instancetype)sharedInstance;
+ (void) writeLog:(GCVLogLevel)logLevel funcName:(const char *)funcName format: (NSString *)fmt, ...;

@end


#endif /* GCVLog_h */
