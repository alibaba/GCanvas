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

#import "WXGCanvasModule.h"
#import "WXGCanvasComponent.h"
#import <GCanvas/GCanvasModule.h>
#import <WeexSDK/WXDefine.h>
#import <WeexSDK/WXComponentManager.h>
#import <SDWebImage/SDWebImageManager.h>
#import <WeexPluginLoader/WeexPluginLoader.h>

@interface WXGCanvasModule()<GCanvasModuleProtocol, GCVImageLoaderProtocol>

@property (nonatomic, strong) GCanvasModule  *gcanvasModule;

@end


@implementation WXGCanvasModule

WX_PlUGIN_EXPORT_MODULE(gcanvas, WXGCanvasModule)

@synthesize weexInstance;

#pragma mark - Export asynchronous method
/**
 * @abstract JS render method
 */
WX_EXPORT_METHOD(@selector(render:componentId:));

/**
 * @abstract JS preload image
 */
WX_EXPORT_METHOD(@selector(preLoadImage:callback:));

/**
 * @abstract JS bind image to a texture
 */
WX_EXPORT_METHOD(@selector(bindImageTexture:componentId:callback:));

/**
 * @abstract JS setContextType,  0-GCVContextType2D, 1-GCVContextTypeWebGL
 */
WX_EXPORT_METHOD(@selector(setContextType:componentId:));

/**
 * @abstract JS setLogLevel
 */
WX_EXPORT_METHOD(@selector(setLogLevel:));

/**
 * @abstract JS call this function while viewdisappear
 */
WX_EXPORT_METHOD(@selector(resetComponent:));



#pragma mark - Export synchronous method
/**
 * @abstract JS initalise GCanvas
 */
WX_EXPORT_METHOD_SYNC(@selector(enable:));

/**
 * @abstract JS callNative method just for WebGL
 */
WX_EXPORT_METHOD_SYNC(@selector(extendCallNative:));



/**
 * WXModuleProtocol method
 * return the execute queue for the module
 */
- (dispatch_queue_t)targetExecuteQueue{
    return [self.gcanvasModule gcanvasExecuteQueue];
}

- (instancetype)init{
    if( self = [super init] ){
        self.gcanvasModule = [[GCanvasModule alloc] initWithDelegate:self];
        self.gcanvasModule.imageLoader = self;
    }
    return self;
}

#pragma mark - Weex Export Method
- (NSString*)enable:(NSDictionary *)args{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onWeexInstanceWillDestroy:)
                                                 name:WX_INSTANCE_WILL_DESTROY_NOTIFICATION
                                               object:nil];
    return [self.gcanvasModule enable:args];
}



/**
 * Export JS method for context 2D render
 *
 * @param   commands    render commands from js
 * @param   componentId GCanvas component identifier
 */
- (void)render:(NSString *)commands componentId:(NSString*)componentId{
    [self.gcanvasModule render:commands componentId:componentId];
}

/**
 * Export JS method for reset GCanvas component while disappear
 *
 * @param   componentId GCanvas component identifier
 */
- (void)resetComponent:(NSString*)componentId{
    [self.gcanvasModule resetComponent:componentId];
}

/**
 * Export JS method for preloading image
 *
 * @param   data        NSArray, contain 2 item
                        data[0] - image source,
                        data[1] - fake texture id(auto-increment id)of GCanvasImage in JS
 * @param   componentId GCanvas component identifier
 * @param   callback
 */
- (void)preLoadImage:(NSArray *)data callback:(WXModuleCallback)callback{
    [self.gcanvasModule preLoadImage:data callback:callback];
}

/**
 * Export JS method for binding image to real native texture
 *
 * @param   data        same as preLoadImage:callback:
 * @param   componentId GCanvas component identifier
 * @param   callback
 */
- (void)bindImageTexture:(NSArray *)data componentId:(NSString*)componentId callback:(WXModuleCallback)callback{
    [self.gcanvasModule bindImageTexture:data componentId:componentId callback:callback];
}

/**
 * Export JS method  set GCanvas plugin contextType
 * @param   type    see GCVContextType
 */
- (void)setContextType:(NSUInteger)type componentId:(NSString*)componentId{
    [self.gcanvasModule setContextType:type componentId:componentId];
}

/**
 * Export JS method  set log level
 * @param   level
 */
- (void)setLogLevel:(NSUInteger)level{
    [self.gcanvasModule setLogLevel:level];
}


#pragma mark - WebGL
/**
 * JS call native directly just for WebGL
 *
 * @param   dict    input WebGL command
                    dict[@"contextId"] - GCanvas component identifier
                    dict[@"type"] - type
                    dict[@"args"] - WebGL command
 
 * @return          return execute result
 */
- (NSDictionary*)extendCallNative:(NSDictionary*)dict{
    return [self.gcanvasModule extendCallNative:dict];
}

#pragma mark - GCanvasModuleProtocol
- (id<GCanvasViewProtocol>)gcanvasComponentById:(NSString*)componentId{
    __block id<GCanvasViewProtocol> component = nil;
    __weak typeof(self) weakSelf = self;

    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    while ( !component || !component.glkview ) {
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.05 * NSEC_PER_SEC)), [self targetExecuteQueue], ^{
            WXPerformBlockSyncOnComponentThread(^{
                component = (WXGCanvasComponent *)[weakSelf.weexInstance componentForRef:componentId];
            });
            dispatch_semaphore_signal(semaphore);
        });
        dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)));
    }
    return component;
}

- (NSString*)gcanvasModuleInstanceId{
    return weexInstance.instanceId;
}

- (void)dispatchGlobalEvent:(NSString*)event params:(NSDictionary*)params{
    [weexInstance fireGlobalEvent:event params:params];
}

#pragma mark - GCVImageLoaderProtocol
- (void)loadImage:(NSURL*)url completed:(GCVLoadImageCompletion)completion{
    [[SDWebImageManager sharedManager].imageDownloader downloadImageWithURL:url options:0 progress:nil completed:^(UIImage *image, NSData *data, NSError *error, BOOL finished) {
            completion(image, error, finished, url);
    }];
}

#pragma mark - Event Notificaiton
- (void)onWeexInstanceWillDestroy:(NSNotification*)notification{
    NSString *instanceId = notification.userInfo[@"instanceId"];
    if (![instanceId isEqualToString:weexInstance.instanceId]) {
        return;
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kGCanvasDestroyNotification
                                                        object:notification.object
                                                      userInfo:notification.userInfo];
}

@end
