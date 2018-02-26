//
//  RCTGCanvasView.h
//  RCTGCanvas
//
//  Created by jiangweixing on 2018/1/10.
//  Copyright © 2018年 taobao. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>
#import <React/RCTView.h>
#import <GCanvas/GCanvasViewProtocol.h>

@class RCTBridge;

@interface RCTGCanvasView : RCTView<GCanvasViewProtocol>

@property (nonatomic, strong, readonly) GLKView* glkview;
@property (nonatomic, assign, readonly) CGRect componetFrame;

/**
 * offscreen UIView<GCanvasViewProtocol>
 */
@property (nonatomic, assign, readonly) BOOL isOffscreen;

/**
 * GCnavas plugin initilaise or refresh
 */
@property (nonatomic, assign) BOOL needChangeEAGLContenxt;


- (CGFloat)devicePixelRatio;
- (NSString*)componentId;


- (instancetype)initWithBridge:(RCTBridge *)bridge;

@end
