/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

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
