//
//  RCTGCanvasView.m
//  RCTGCanvas
//
//  Created by jiangweixing on 2018/1/10.
//  Copyright © 2018年 taobao. All rights reserved.
//

#import "RCTGCanvasView.h"

#import <React/RCTBridge.h>
#import <React/RCTUtils.h>
#import <React/UIView+React.h>

@interface RCTGCanvasView()

@property (nonatomic, strong)  GLKView *glkview;

@end

@implementation RCTGCanvasView
{
    __weak RCTBridge *_bridge;
}


- (instancetype)initWithBridge:(RCTBridge *)bridge{
    if (self = [super init]) {
        _bridge = bridge;
    }
    return self;
}

- (void)reactSetFrame:(CGRect)frame{
    [super reactSetFrame:frame];
    
    if( !self.glkview ){
        self.glkview = [[GLKView alloc] init];
        self.glkview.enableSetNeedsDisplay = YES;
        self.glkview.userInteractionEnabled = YES;
        self.glkview.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        self.glkview.backgroundColor = [UIColor clearColor];
        [self addSubview:self.glkview];
    }
    
    self.glkview.frame = self.bounds;
    _componetFrame = frame;
    self.needChangeEAGLContenxt = YES;
}

- (CGFloat)devicePixelRatio{
    return  [UIScreen mainScreen].nativeScale;
}

- (NSString*)componentId{
    return [NSString stringWithFormat:@"%@", self.reactTag];
}

@end
