//
//  RCTGCanvasModule.m
//  RCTGCanvas
//
//  Created by jiangweixing on 2018/1/9.
//  Copyright © 2018年 taobao. All rights reserved.
//

#import "RCTGCanvasModule.h"
#import "RCTGCanvasView.h"

#import <GCanvas/GCanvasModule.h>

#import <GLKit/GLKit.h>
#import <React/RCTConvert.h>
#import <React/RCTLog.h>
#import <React/RCTUtils.h>
#import <React/RCTUIManagerUtils.h>
#import <React/UIView+React.h>
#import <React/RCTUIManager.h>
#import <React/RCTImageLoader.h>


@interface RCTGCanvasModule()<GCanvasModuleProtocol, GCVImageLoaderProtocol>

@property (nonatomic, strong) GCanvasModule  *gcanvasModule;

@end

@implementation RCTGCanvasModule


RCT_EXPORT_MODULE()

+ (BOOL)requiresMainQueueSetup
{
    return NO;
}
- (instancetype)init{
    if( self = [super init] ){
        self.gcanvasModule = [[GCanvasModule alloc] initWithDelegate:self];
        self.gcanvasModule.imageLoader = self;
    }
    return self;
}

- (dispatch_queue_t)methodQueue
{
    static dispatch_queue_t queue;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        queue = dispatch_queue_create("com.facebook.react.gcanvasQueue", DISPATCH_QUEUE_SERIAL);
    });
    return queue;
}

- (NSArray<NSString *> *)supportedEvents{
    return @[@"GCanvasReady"];
}


#pragma mark - Export Method
RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(enable:(NSDictionary*) args)
{
    return [self.gcanvasModule enable:args];
}

//render
RCT_EXPORT_METHOD(render:(NSString *)commands componentId:(NSString*)componentId)
{
    [self.gcanvasModule render:commands componentId:componentId];
}

//preload image
RCT_EXPORT_METHOD(preLoadImage:(NSArray *)data callback:(RCTResponseSenderBlock)callback)
{
    [self.gcanvasModule preLoadImage:data callback:^(id result) {
        callback(@[result?:@""]);
    }];
}

//bind image texture
RCT_EXPORT_METHOD(bindImageTexture:(NSArray *)data componentId:(NSString*)componentId callback:(RCTResponseSenderBlock)callback)
{
    [self.gcanvasModule bindImageTexture:data componentId:componentId callback:^(id result) {
        callback(@[result?:@""]);
    }];
}

//set context type
RCT_EXPORT_METHOD(setContextType:(NSUInteger)type componentId:(NSString*)componentId)
{
    [self.gcanvasModule setContextType:type componentId:componentId];
}

//set loglevel
RCT_EXPORT_METHOD(setLogLevel:(NSUInteger)level)
{
    [self.gcanvasModule setLogLevel:level];
}

//reset component
RCT_EXPORT_METHOD(resetComponent:(NSString*)componentId)
{
    [self.gcanvasModule resetComponent:componentId];
}

#pragma mark - WebGL
RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(extendCallNative:(NSDictionary*)dict)
{
    return [self.gcanvasModule extendCallNative:dict];
}

#pragma mark - GCVImageLoaderProtocol
- (void)loadImage:(NSURL*)url completed:(GCVLoadImageCompletion)completion{
    NSURLRequest *requst = [NSURLRequest requestWithURL:url];
    [self.bridge.imageLoader loadImageWithURLRequest:requst callback:^(NSError *error, UIImage *image) {
        completion(image, error, !error, url);
    }];
}


#pragma mark - GCanvasModuleProtocol
- (NSString*)gcanvasModuleInstanceId
{
    return [NSString stringWithFormat:@"%p", self];
}

- (id<GCanvasViewProtocol>)gcanvasComponentById:(NSString*)componentId
{
    NSNumber *reactTag = [NSNumber numberWithInteger: [componentId integerValue]];
    RCTUIManager *uiManager = self.bridge.uiManager;
    
    __block UIView *view = nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        view = [uiManager viewForReactTag:reactTag];
    });
    if ( [view isKindOfClass: [RCTGCanvasView class]] ){
        return (RCTGCanvasView*)view;
    }
    return nil;
}

- (void)dispatchGlobalEvent:(NSString*)event params:(NSDictionary*)params
{
    [self sendEventWithName:event body:params];
}

@end
