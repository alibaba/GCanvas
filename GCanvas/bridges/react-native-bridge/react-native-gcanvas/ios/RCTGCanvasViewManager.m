//
//  RCTGCanvasViewManager.m
//  RCTGCanvas
//
//  Created by jiangweixing on 2018/1/10.
//  Copyright © 2018年 taobao. All rights reserved.
//

#import "RCTGCanvasViewManager.h"
#import "RCTGCanvasView.h"
#import <React/UIView+React.h>

@implementation RCTGCanvasViewManager

@synthesize bridge = _bridge;


RCT_EXPORT_MODULE()

- (UIView *)view{
    return (UIView*)[[RCTGCanvasView alloc] initWithBridge:self.bridge];
}

RCT_EXPORT_VIEW_PROPERTY(isOffscreen, NSUInteger)

@end
