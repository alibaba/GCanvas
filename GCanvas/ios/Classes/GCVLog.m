/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#import "GCVCommon.h"

@implementation GCVLog

+ (instancetype)sharedInstance{
    static GCVLog *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[self alloc] init];
        sharedInstance.logLevel = GCVLogLevelInfo;
    });
    return sharedInstance;
}

+ (void) writeLog:(GCVLogLevel)logLevel funcName:(const char *)funcName format: (NSString *)fmt, ...{
    if ( logLevel < [GCVLog sharedInstance].logLevel ){
        return;
    }
    
    static int logLimit = 256;
    va_list va;
    va_start(va, fmt);
    NSString *fullLog = [[NSString alloc] initWithFormat:fmt arguments:va];
    if ( fullLog.length > logLimit ){
        fullLog = [NSString stringWithFormat:@"[leng:%lu] %@", (unsigned long)fullLog.length, [fullLog substringToIndex:logLimit]];
    }
    
    if ( strlen(funcName)>64 ){
        NSLog(@"%.64s...] %@", funcName, fullLog);
    } else {
        NSLog(@"%s %@", funcName, fullLog);
    }
    va_end(va);
}

@end
