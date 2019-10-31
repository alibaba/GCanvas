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

#import "GCanvasModule.h"
#import "GCanvasObject.h"
#import "GCanvasViewProtocol.h"
#import "GCVCommon.h"

#define kGCanvasOffScreenPrefix @"offscreen_"

#ifndef dispatch_main_sync_safe
#define dispatch_main_sync_safe(block)\
    if ([NSThread isMainThread]) {\
    block();\
    } else {\
    dispatch_sync(dispatch_get_main_queue(), block);\
    }
#endif

#ifndef dispatch_main_async_safe
#define dispatch_main_async_safe(block)\
    if ([NSThread isMainThread]) {\
    block();\
    } else {\
    dispatch_async(dispatch_get_main_queue(), block);\
    }
#endif

@interface GCanvasModule()<GLKViewDelegate>

/**
 * cache GCanvasObject by componentId
 */
@property (nonatomic, strong) NSMutableDictionary *gcanvasObjectDict;

/**
 * enter background flag
 */
@property (nonatomic, assign) BOOL enterBackground;

/**
 * dispatch queue only for preload image
 */
@property (nonatomic, strong) dispatch_queue_t preloadQueue;

@end

@implementation GCanvasModule

#pragma mark - CreateEAGLContext
/**
 * Create EAGLContext by the same EAGLSharegroup
 */
static EAGLContext          *_staticFirstContext;
static NSMutableDictionary  *_staticModuleExistDict;
+ (EAGLContext*)createEAGLContextWithModuleInstance:(NSString*)instanceId{
    EAGLContext *context = nil;
    if( !_staticFirstContext ){
        _staticFirstContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        context = _staticFirstContext;
    } else {
        EAGLContext *newContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:_staticFirstContext.sharegroup];
        context = newContext;
    }
    
    if( !_staticModuleExistDict ){
        _staticModuleExistDict = NSMutableDictionary.dictionary;
    }
    
    if( !_staticModuleExistDict[instanceId] ){
        _staticModuleExistDict[instanceId] = @(1);
    }
    
    return context;
}

- (instancetype)initWithDelegate:(nonnull id<GCanvasModuleProtocol>)delegate{
    if( self = [super init] ){
        _deletage = delegate;
    }
    return self;
}

- (void)dealloc{
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

/**
 * return the execute queue for the module
 */
- (dispatch_queue_t)gcanvasExecuteQueue{
    static dispatch_queue_t gcanvasQueue;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        gcanvasQueue = dispatch_queue_create("com.taobao.gcanvas", DISPATCH_QUEUE_SERIAL);
    });
    return gcanvasQueue;
}

#pragma mark - Need Export Context2D Method
- (NSString*)enable:(NSDictionary *)args{
    if (!args || !args[@"componentId"] ){
        return @"false";
    }
    
    NSString *componentId = args[@"componentId"];
    GCVLOG_METHOD(@"enable:, componentId=%@", componentId);
    
    if( !self.gcanvasObjectDict ){
        self.gcanvasObjectDict = NSMutableDictionary.dictionary;
        self.enterBackground = NO;
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onGCanvasCompLoadedNotify:)
                                                     name:kGCanvasCompLoadedNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onGCanvasResetNotify:)
                                                     name:kGCanvasResetNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onInstanceWillDestroy:)
                                                     name:kGCanvasDestroyNotification
                                                   object:nil];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onDidEnterBackgroundNotify:)
                                                     name:UIApplicationWillResignActiveNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(onWillEnterForegroundNotify:)
                                                     name:UIApplicationDidBecomeActiveNotification
                                                   object:nil];
        GCVWeakSelf
        [GCanvasPlugin setFetchPlugin:^GCanvasPlugin *(NSString *componentId) {
            return [weakSelf gcanvasPluginById:componentId];
        }];
    }
    
    if( self.gcanvasObjectDict[componentId] ){
        return @"true";
    }
    
    GCanvasObject *gcanvasInst = [[GCanvasObject alloc] initWithComponentId:componentId];
    self.gcanvasObjectDict[componentId] = gcanvasInst;
    
    GCanvasPlugin *plugin = [[GCanvasPlugin alloc] initWithComponentId:componentId];
    gcanvasInst.plugin = plugin;
    
    GCVWeakSelf
    id<GCanvasViewProtocol> component = [self.deletage gcanvasComponentById:componentId];
    if( component ){
        dispatch_sync([self gcanvasExecuteQueue], ^{
            EAGLContext *context = [GCanvasModule createEAGLContextWithModuleInstance:[weakSelf.deletage gcanvasModuleInstanceId]];
            context.multiThreaded = YES;
            component.glkview.context = context;
            component.glkview.delegate = weakSelf;
        });
        gcanvasInst.component = component;
        
        [plugin setGLKView:component.glkview];
    }
    return @"true";
}
/**
 * Export JS method for context 2D render
 *
 * @param   commands    render commands from js
 * @param   componentId GCanvas component identifier
 */
- (void)render:(NSString *)commands componentId:(NSString*)componentId{
    if( self.enterBackground ) return;
    
    GCVLOG_METHOD(@"render:componentId:,commands=%@, componentId=%@", commands, componentId);
    
    GCanvasObject *gcanvasInst = self.gcanvasObjectDict[componentId];
    id<GCanvasViewProtocol> component = gcanvasInst.component;
    GCanvasPlugin *plugin = gcanvasInst.plugin;
    if( !component || !plugin ){
        return;
    }
    
    if( component.isOffscreen ){
        component.glkview.hidden = YES;
    }
    
    [plugin addCommands:commands];
    [self execCommandById:componentId];
}

/**
 * Export JS method for reset GCanvas component while disappear
 * [iOS only]
 *
 * @param   componentId GCanvas component identifier
 */
- (void)resetComponent:(NSString*)componentId{
    GCVLOG_METHOD(@"resetComponent:,componentId=%@", componentId);
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kGCanvasResetNotification
                                                        object:nil
                                                      userInfo:@{@"componentId":componentId}];
}

/**
 * Export JS method for preloading image
 *
 * @param   data        NSArray, contain 2 item
 *          data[0]     image source,
 *          data[1]     fake texture id(auto-increment id)of GCanvasImage in JS
 * @param   callback    GCanvasModuleCallback callback
 */
- (void)preLoadImage:(NSArray *)data callback:(GCanvasModuleCallback)callback{
    if( !data || ![data isKindOfClass:NSArray.class] || data.count != 2){
        if( callback ) callback(@{@"error":@"Input Param Error"});
        return;
    }
    
    if( ![GCVCommon sharedInstance].imageLoader ){
        [GCVCommon sharedInstance].imageLoader = self.imageLoader;
    }
    
    if( !self.preloadQueue ){
        self.preloadQueue = dispatch_queue_create("com.taobao.gcanvas.preload", DISPATCH_QUEUE_CONCURRENT);
    }
    
    NSString *src = data[0];
    if( [src hasPrefix:kGCanvasOffScreenPrefix] ){
        if( callback ) callback(@{});
        return;
    }
    
    GCVLOG_METHOD(@"preLoadImage:callback:, src=%@", src);
    dispatch_async(self.preloadQueue , ^{
        NSUInteger jsTextureId = [data[1] integerValue];
        [[GCVCommon sharedInstance] addPreLoadImage:src completion:^(GCVImageCache *imageCache, BOOL fromCache) {
            if ( !imageCache ){
                if( callback ) callback(@{@"error":@"Preload Image Error"});
                return;
            }
            imageCache.jsTextreId = jsTextureId;
            if( callback) {
                callback(@{@"width":@(imageCache.width), @"height":@(imageCache.height)});
            }
        }];
    });
}

/**
 * Export JS method for binding image to real native texture
 *
 * @param   data        NSArray, contain 2 item
 *          data[0]     image source,
 *          data[1]     fake texture id(auto-increment id)of GCanvasImage in JS
 * @param   componentId GCanvas component identifier
 * @param   callback    GCanvasModuleCallback callback
 */
- (void)bindImageTexture:(NSArray *)data componentId:(NSString*)componentId callback:(GCanvasModuleCallback)callback{
    if( !data || ![data isKindOfClass:NSArray.class] || data.count != 2){
        if( callback ) callback(@{@"error":@"Input Param Error"});
        return;
    }
    
    GCanvasObject *gcanvasInst = self.gcanvasObjectDict[componentId];
    GCanvasPlugin *plugin = gcanvasInst.plugin;
    id<GCanvasViewProtocol> component = gcanvasInst.component;
    if( !component || !plugin ){
        if( callback ) callback(@{});
        return;
    }
    
    NSString *src = data[0];
    NSUInteger jsTextureId = [data[1] integerValue];
    
    GCVLOG_METHOD(@"bindImageTexture:componentId:callback:, src=%@", src);
    
    __block GLuint textureId = [plugin getTextureId:jsTextureId];
    if( textureId == 0 ){
        //check offscreen
        NSRange range = [src rangeOfString:kGCanvasOffScreenPrefix];
        if( range.location == 0 ){
            NSString *orgComponentId = [src substringFromIndex:range.length];
            GCanvasObject *gcanvasInst = self.gcanvasObjectDict[orgComponentId];
            GCanvasPlugin *orgPlugin = gcanvasInst.plugin;
            id<GCanvasViewProtocol> orgComponent = gcanvasInst.component;
            if( orgPlugin  && orgComponent ){
                orgComponent.glkview.hidden = YES;
                [plugin addTextureId:[orgPlugin textureId] withAppId:jsTextureId
                               width:orgComponent.componetFrame.size.width
                              height:orgComponent.componetFrame.size.height
                           offscreen:YES];
            }
            if( callback ) callback(@{});
            return;
        }
        
        void (^bindImageTextureBlock)(GCVImageCache*) = ^(GCVImageCache* cache){
            dispatch_main_async_safe(^{
                [EAGLContext setCurrentContext:component.glkview.context];
                textureId = [GCVCommon bindTexture:cache.image];
                if( textureId > 0 ){
                    GCVLOG_METHOD(@"==>bindImageTexture success: jsTextureId:%d => texutreId:%d, componentId:%@", jsTextureId, textureId, componentId);
                    
                    [plugin addTextureId:textureId withAppId:jsTextureId
                                   width:cache.width height:cache.height
                               offscreen:NO];
                    [[GCVCommon sharedInstance] removeLoadImage:src];
                }
            });
        };
        
        GCVImageCache *imageCache = [[GCVCommon sharedInstance] fetchLoadImage:src];
        if( !imageCache ){
            [[GCVCommon sharedInstance] addPreLoadImage:src completion:^(GCVImageCache *imageCache, BOOL fromCache) {
                bindImageTextureBlock(imageCache);
                if( callback ){
                    (textureId > 0) ? callback(@{}) : callback(@{@"error":@"Bind Image To Texture Error"});
                }
            }];
            return;
        } else {
            bindImageTextureBlock(imageCache);
        }
    }
    
    if( callback ){
        (textureId > 0) ? callback(@{}) : callback(@{@"error":@"Bind Image To Texture Error"});
    }
}

/**
 * Export JS method  set GCanvas plugin contextType
 * @param   type    see GCVContextType
 */
- (void)setContextType:(NSUInteger)type componentId:(NSString*)componentId{
    GCVLOG_METHOD(@"setContextType:componentId:, type:%d, componentId:%@", (unsigned long)type, componentId);
    GCanvasObject *gcanvasInst = self.gcanvasObjectDict[componentId];
    GCanvasPlugin *plugin = gcanvasInst.plugin;
    if( plugin ){
        [plugin setContextType:(int)type];
    }
}

/**
 * Export JS method  set log level
 * @param   level  loglevel 0-debug,1-info,2-warn,3-error
 */
- (void)setLogLevel:(NSUInteger)level{
    GCVLOG_METHOD(@"setLogLevel:, loglevel:%d", level);

    [GCanvasPlugin setLogLevel:level];
}

#pragma mark - Notification
- (void)onGCanvasCompLoadedNotify:(NSNotification*)notification{
    NSLog(@"onGCanvasCompLoadedNotify...");
}

- (void)onGCanvasResetNotify:(NSNotification*)notification{
    NSString *componentId = notification.userInfo[@"componentId"];
    
    //find plugin and component bind with componentId, set needChangeEAGLContenxt and remove render commands
    [self.gcanvasObjectDict enumerateKeysAndObjectsUsingBlock:^(NSString *compId, GCanvasObject *gcanvsInst, BOOL * _Nonnull stop) {
        if ( [componentId isEqualToString:gcanvsInst.componentId] &&  gcanvsInst.component ) {
            gcanvsInst.component.needChangeEAGLContenxt = YES;
            GCanvasPlugin *plugin = gcanvsInst.plugin;
            [plugin removeCommands];
        }
    }];
}

- (void)onDidEnterBackgroundNotify:(NSNotification*)notification{
    self.enterBackground = YES;
}

- (void)onWillEnterForegroundNotify:(NSNotification*)notification{
    self.enterBackground = NO;
}

- (void)onInstanceWillDestroy:(NSNotification*)notification{
    NSString *instanceId = notification.userInfo[@"instanceId"];
    if( ![instanceId isEqualToString:[self.deletage gcanvasModuleInstanceId]] ){
        return;
    }
    
    [self.gcanvasObjectDict enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, GCanvasObject* gcanvasInst, BOOL * _Nonnull stop) {
        id<GCanvasViewProtocol> comp = gcanvasInst.component;
        comp.glkview.delegate = nil;
        
        GCanvasPlugin *plugin = gcanvasInst.plugin;
        [plugin removeGCanvas];
    }];
    
    [self.gcanvasObjectDict removeAllObjects];
    self.gcanvasObjectDict = nil;
    
    [[GCVCommon sharedInstance] clearLoadImageDict];
    
    [_staticModuleExistDict removeObjectForKey:instanceId];
    if( _staticModuleExistDict.count == 0 ){
        _staticFirstContext = nil;
    }
}

#pragma mark - Private
- (GCanvasPlugin*)gcanvasPluginById:(NSString*)componentId{
    GCanvasObject *gcanvasInst = self.gcanvasObjectDict[componentId];
    GCanvasPlugin *plugin = gcanvasInst.plugin;
    return plugin;
}

- (GCanvasObject*)gcanvasInstanceByGLKView:(GLKView*)glkview{
    __block GCanvasObject *gcanvasInst = nil;
    [self.gcanvasObjectDict enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, GCanvasObject *obj, BOOL * _Nonnull stop) {
        if( obj.component.glkview == glkview ){
            gcanvasInst = obj;
            *stop = YES;
        }
    }];
    return gcanvasInst;
}

- (void)execCommandById:(NSString*)componentId{
    GCVLOG_METHOD(@"execCommandById:, componentId: %@",componentId);
    
    GCanvasObject *gcanvasInst = self.gcanvasObjectDict[componentId];
    if ( gcanvasInst.component ) {
        GCVWeakSelf
        dispatch_main_async_safe(^{
            if( !weakSelf.enterBackground ){
                [gcanvasInst.component.glkview setNeedsDisplay];
            }
        });
    }
}

/**
 * reset and refresh GCanvas Plugin
 *
 * @param   plugin      the GCanvasPlugin object to refresh
 * @param   component   id<GCanvasViewProtocol component bind with plugin
 */
- (void)refreshPlugin:(GCanvasPlugin*)plugin withComponent:(id<GCanvasViewProtocol>)component{
    CGFloat devicePixelRatio = 1.0;
    if( plugin.contextType == GCVContextType2D ){
        devicePixelRatio = component.devicePixelRatio;
    }
    [plugin setDevicePixelRatio:devicePixelRatio];
    
    CGRect compFrame = component.componetFrame;
    CGRect gcanvasFrame = CGRectMake(compFrame.origin.x, compFrame.origin.y,
                                     compFrame.size.width*devicePixelRatio,
                                     compFrame.size.height*devicePixelRatio);
    [plugin setClearColor:component.glkview.backgroundColor];
    [plugin setFrame:gcanvasFrame];
}

#pragma mark - GLKViewDelegate
- (void)glkView:(GLKView*)view drawInRect:(CGRect)rect{
    GCanvasObject *gcanvasInst = [self gcanvasInstanceByGLKView:view];
    
    id<GCanvasViewProtocol> component = gcanvasInst.component;
    GCanvasPlugin *plugin = gcanvasInst.plugin;
    
    if( !component || !plugin ){
        return;
    }
    
    GCVLOG_METHOD(@"glkView:drawInRect:, componentId:%@, context:%p", component.componentId, component.glkview.context);
    
    //multi GCanvas instance, need change current context while execute render commands
    [EAGLContext setCurrentContext:component.glkview.context];
    
    if ( component.needChangeEAGLContenxt ){
        [self refreshPlugin:plugin withComponent:component];
        if( [self.deletage respondsToSelector:@selector(dispatchGlobalEvent:params:)] ){
            [self.deletage dispatchGlobalEvent:@"GCanvasReady" params:@{@"ref":component.componentId}];
        }
        component.needChangeEAGLContenxt = NO;
    }
    
    [plugin execCommands];
}

#pragma mark - Need Export WebGL Method
/**
 * JS call native directly just for WebGL
 *
 * @param   dict    input WebGL command
 *          dict[@"contextId"] - GCanvas component identifier
 *          dict[@"type"] - type
 *          dict[@"args"] - WebGL command
 *
 * @return          return execute result
 */
- (NSDictionary*)extendCallNative:(NSDictionary*)dict{
    NSString *componentId = dict[@"contextId"];
    GCanvasObject *gcanvasInst = self.gcanvasObjectDict[componentId];
    if( !gcanvasInst ){
        return @{};
    }
    
    //WebGL no need set glkview delegate
    id<GCanvasViewProtocol> component = gcanvasInst.component;
    if( component.glkview.delegate ){
        component.glkview.delegate = nil;
    }
    
    NSDictionary *retDict = [self callGCanvasNative:dict];
    return retDict;
}

- (NSDictionary*)callGCanvasNative:(NSDictionary*)dict{
    NSString *componentId = dict[@"contextId"];
    NSUInteger type = [dict[@"type"] integerValue];
    NSString *args = dict[@"args"];
    
    GCanvasObject *gcanvasInst = self.gcanvasObjectDict[componentId];
    
    id<GCanvasViewProtocol> component = gcanvasInst.component;
    GCanvasPlugin *plugin = gcanvasInst.plugin;
    
    if( !component || !plugin ) return @{};
    
    if ( component.needChangeEAGLContenxt ){
        [EAGLContext setCurrentContext:component.glkview.context];
        
        [self refreshPlugin:plugin withComponent:component];
        component.needChangeEAGLContenxt = NO;
        
        //setNeedsDisplay at first, for WebGL case render only once.
        dispatch_main_sync_safe(^{
            [component.glkview setNeedsDisplay];
        });
    }
    
    /* call native type description
     +-----------------------------------------------------+
     |                   32 bit integer                    |
     +-----------------------------------------------------+
     |    31~30    |     29      |       (28~0)            |
     | ContextType | Method Type |      OptionType         |
     +-----------------------------------------------------+
     |  0x01-WebGL |  0x00-async | 0-defaut,1-WebGL render |
     |  0x00-2D    |  0x01-sync  | other reserve           |
     +-----------------------------------------------------+
     */
    if( (type >> 30 & 0x01) == 1 ){      //webgl
        BOOL isSync = type >> 29 & 0x01; //sync
        if( isSync ){
            BOOL rendCmd = type & 0x01;  //render per 16 ms
            if( rendCmd ){
                dispatch_main_sync_safe(^{
                    [component.glkview setNeedsDisplay];
                });
                return @{};
            }
        //#ifdef DEBUG
            else{
                NSRange range = [args rangeOfString:@","];
                if (range.location != NSNotFound) {
                    NSString *indexStr = [args substringToIndex:range.location];
                    NSUInteger index = [indexStr integerValue];
                    if (index == 136) {
                        dispatch_main_sync_safe(^{
                            [component.glkview setNeedsDisplay];
                        });
                    }
                }
            }
        //#endif
            
            [plugin addCommands:args];
            [plugin execCommands];
            
            NSString *ret = [plugin getSyncResult];
            if(ret){
                return @{@"result":ret};
            }
        }
    }
    return @{};
}

@end
