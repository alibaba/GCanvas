/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#import "GCanvasPlugin.h"
#import "GCVCommon.h"
#import "GCVFont.h"
#import "GCVLog.h"

#include "GCanvasWeex.hpp"
#include "GCanvas2dContext.h"
#include "GCanvasManager.h"
#include "Log.h"


@interface GCanvasPlugin()

@property(nonatomic, strong) NSString *componentId;
@property(nonatomic, assign) gcanvas::GCanvasWeex *gcanvas;
@property(nonatomic, assign) GCanvasContext *context;
@property(nonatomic, strong) NSMutableArray *renderCommandArray;
@property(nonatomic, strong) NSMutableDictionary *textureDict;

@property(nonatomic, weak) GLKView *glkview;

@end

@implementation GCanvasPlugin

+ (void)setLogLevel:(NSUInteger)logLevel{
    if( logLevel < LOG_LEVEL_DEBUG || logLevel > LOG_LEVEL_FATAL ) return;
#ifdef DEBUG
    [GCVLog sharedInstance].logLevel = (GCVLogLevel)logLevel;
#endif
    SetLogLevel((LogLevel)logLevel);
}

+ (FetchPluginBlock)GetFetchPluginBlock{
    static FetchPluginBlock gFetchPluginBlock;
    return gFetchPluginBlock;
}

+ (void)setFetchPlugin:(FetchPluginBlock)block
{
    FetchPluginBlock fetchPluginBlock = [self GetFetchPluginBlock];
    fetchPluginBlock = block;
}


- (instancetype)initWithComponentId:(NSString*)componentId{
    self = [super init];
    if (self){
        self.renderCommandArray = [[NSMutableArray alloc] init];
        self.textureDict = NSMutableDictionary.dictionary;
        
        self.gcanvasInited = NO;
        self.componentId = componentId;
        
        gcanvas::GCanvasManager* manager = gcanvas::GCanvasManager::GetManager();
        std::string key = [componentId UTF8String];

        self.gcanvas = manager->NewCanvasWeex(key);
        self.context = self.gcanvas->GetGCanvasContext();
                
    #ifdef DEBUG
        SetLogLevel(LOG_LEVEL_DEBUG);
    #else
        SetLogLevel(LOG_LEVEL_INFO);
    #endif
    }
    return self;
}

- (void)dealloc{
    [self removeGCanvas];
}

- (void)setFrame:(CGRect)frame{
    if( !self.gcanvas ) return;
    if( !self.gcanvasInited ){
        self.gcanvas->OnSurfaceChanged(0, 0, frame.size.width, frame.size.height);
        self.gcanvasInited = YES;
    }
}

- (void)setClearColor:(UIColor*)color{
    if( !self.gcanvas ) return;
    
    if( color ){
        CGFloat r, g, b, a;
        BOOL ret = [color getRed:&r green:&g blue:&b alpha:&a];
        if( ret ){
            GColorRGBA c;
            c.rgba.r = r;
            c.rgba.g = g;
            c.rgba.b = b;
            c.rgba.a = a;
            self.gcanvas->SetClearColor(c);
        }
    }
}

- (void)addCommands:(NSString*)commands{
    if (commands){
        @synchronized (self) {
            [self.renderCommandArray addObject:commands];
        }
    }
}

- (void)removeCommands{
    @synchronized (self) {
        [self.renderCommandArray removeAllObjects];
    }
}

- (void)execCommands{
    if( !self.gcanvas || !self.gcanvasInited ) return;
    
    @synchronized (self) {
        if(self.renderCommandArray.count == 0){
            return;
        }
        NSString* cmd = self.renderCommandArray[0];
        int cmdLen = (int)strlen(cmd.UTF8String);
        GCVLOG_METHOD(@"execCommands, len:%d, command=%@", cmdLen, cmd);
        self.gcanvas->Render(cmd.UTF8String, cmdLen);
        [self.renderCommandArray removeObjectAtIndex:0];
    }
}

- (void)releaseManager{
    gcanvas::GCanvasManager::Release();
}

- (void)removeGCanvas{
    gcanvas::GCanvasManager* manager = gcanvas::GCanvasManager::GetManager();
    manager->RemoveCanvas([self.componentId UTF8String]);
    
    @synchronized(self){
        [self.textureDict enumerateKeysAndObjectsUsingBlock:^(id  key, id value, BOOL * _Nonnull stop) {
            GLuint tid = (GLuint)[value integerValue];
            if(tid > 0){
                glDeleteTextures(1, &tid);
            }
        }];
        [self.textureDict removeAllObjects];
        self.textureDict = nil;
    }
    
    self.gcanvas = nil;
}

- (GLuint)getTextureId:(NSUInteger)aid{
    if( !self.gcanvas ) return 0;
    
    @synchronized(self){
        GLuint tid = (GLuint)([self.textureDict[@(aid)] integerValue]);
        return tid;
    }
}

- (void)addTextureId:(NSUInteger)tid withAppId:(NSUInteger)aid width:(NSUInteger)width height:(NSUInteger)height offscreen:(BOOL)offscreen{
    if( !self.gcanvas ) return;
    
    self.gcanvas->AddTexture((int)aid, (int)tid, (int)width, (int)height);
    if( offscreen ){
        self.gcanvas->AddOfflineTexture((int)aid, (int)tid);
    }
    @synchronized(self){
        self.textureDict[@(aid)] = @(tid);
    }
}

- (void)addTextureId:(NSUInteger)tid withAppId:(NSUInteger)aid width:(NSUInteger)width height:(NSUInteger)height{
    [self addTextureId:tid withAppId:aid width:width height:height offscreen:NO];
}

- (void)setImageLoader:(id<GCVImageLoaderProtocol>)loader{
    [GCVCommon sharedInstance].imageLoader = loader;
}

- (void)setDevicePixelRatio:(double)ratio{
    if( !self.gcanvas ) return;

    self.gcanvas->SetDevicePixelRatio(ratio);
}

- (void)setContextType:(GCVContextType)contextType{
    if( !self.gcanvas ) return;
    
    self.gcanvas->SetContextType((GCVContextTypeWebGL == contextType ) ? GCVContextTypeWebGL : GCVContextType2D);
    
    //register function for GFont and WebGL texImage2D && texSubImage2D.
    if( self.gcanvas->GetContextType() != 0 ){
        self.context->SetGWebGLTxtImage2DFunc(iOS_GCanvas_GWebGLTxtImage2D);
        self.context->SetGWebGLTxtSubImage2DFunc(iOS_GCanvas_GWebGLTxtSubImage2D);
        self.context->SetGWebGLBindToGLKViewFunc(iOS_GCanvas_Bind_To_GLKView);
    }
}

- (int)contextType{
    if( !self.gcanvas ) return 0;
    return self.gcanvas->GetContextType();
}

- (CGFloat)fps{
    if( !self.gcanvas ) return 0;
    return self.gcanvas->mFps;
}

- (GLuint)textureId{
    if( !self.gcanvas ) return 0;
    return self.gcanvas->GetFboTexture()->GetTextureID();
}

- (NSString*)getSyncResult{
    if( !self.gcanvas ) return @"";

   if( !self.gcanvas->mResult.empty() ){
        NSString *retResult = [[NSString alloc] initWithUTF8String:self.gcanvas->mResult.c_str()];
        self.gcanvas->setSyncResult("");
        return retResult;
    }
    return @"";
}

- (void)setGLKView:(GLKView*)glkview{
    self.glkview = glkview;
}

- (GLKView*)getGLKView{
    return self.glkview;
}



#pragma mark - iOS implementation of Font & texImage
/**
 * Fetch GCVImageCache by image source, create texImage2D with imageData.
 * create textImage2D success, while imageCache loaded.
 *
 * @param target            texutre target to render
 * @param level             level
 * @param internalformat    internalformat
 * @param format            format
 * @param type              type
 * @param src               image src to loader
 *
 */
void iOS_GCanvas_GWebGLTxtImage2D(GLenum target, GLint level, GLenum internalformat,
                                  GLenum format, GLenum type,  const char *src){
    NSString *imageStr = [[NSString alloc] initWithUTF8String:src];
    GCVImageCache *imageCache = [[GCVCommon sharedInstance] fetchLoadImage:imageStr];
    
    void (^glTexImage2DBlock)(GCVImageCache*) = ^(GCVImageCache* cache){
        LOG_D("width=%f, height=%f, image=%p", imageCache.width, imageCache.height, imageCache.image);
        
        GLint width = imageCache.width;
        GLint height = imageCache.height;
        
        CGRect rect = CGRectMake(0, 0, width, height);
        CGBitmapInfo info =  kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
        NSMutableData *pixelData = [NSMutableData dataWithLength:width*height*4];
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate(pixelData.mutableBytes, width, height, 8, 4 * width, colorSpace, info);
        CGColorSpaceRelease(colorSpace);
        CGContextClearRect(context, rect);
        CGContextDrawImage(context, rect, imageCache.image.CGImage);
        CGContextRelease(context);
        
        glTexImage2D(target, level, internalformat, width, height, 0, format, type, pixelData.bytes);
    };
    
    if( imageCache ) {
        glTexImage2DBlock(imageCache);
    } else {
        [[GCVCommon sharedInstance] addPreLoadImage:imageStr completion:^(GCVImageCache *imageCache, BOOL fromCache){
            if( imageCache ){
                glTexImage2DBlock(imageCache);
            }
        }];
    }
}

/**
 * Fetch GCVImageCache by image source, create texImage2D with imageData.
 * create textImage2D success, while imageCache loaded.
 *
 * @param target            texutre target to render
 * @param level             level
 * @param xoffset           xoffset
 * @param yoffset           yoffset
 * @param format            format
 * @param type              type
 * @param src               image src to loader
 *
 */
void iOS_GCanvas_GWebGLTxtSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset,
                                     GLenum format, GLenum type,  const char *src){
    NSString *imageStr = [[NSString alloc] initWithUTF8String:src];
    GCVImageCache *imageCache = [[GCVCommon sharedInstance] fetchLoadImage:imageStr];
    
    void (^glTexSubImage2DBlock)(GCVImageCache*) = ^(GCVImageCache* cache){
        LOG_D("width=%f, height=%f, image=%p", imageCache.width, imageCache.height, imageCache.image);
        
        GLint width = imageCache.width;
        GLint height = imageCache.height;
        
        CGRect rect = CGRectMake(0, 0, width, height);
        CGBitmapInfo info =  kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
        NSMutableData *pixelData = [NSMutableData dataWithLength:width*height*4];

        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate(pixelData.mutableBytes, width, height, 8, 4 * width, colorSpace, info);
        CGColorSpaceRelease(colorSpace);
        CGContextClearRect(context, rect);
        CGContextDrawImage(context, rect, imageCache.image.CGImage);
        CGContextRelease(context);
        
        glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixelData.bytes);
    };

    if( imageCache ){
        glTexSubImage2DBlock(imageCache);
    } else {
        [[GCVCommon sharedInstance] addPreLoadImage:imageStr completion:^(GCVImageCache *imageCache, BOOL fromCache){
            if( imageCache ){
                glTexSubImage2DBlock(imageCache);
            }
        }];
    }
}

void iOS_GCanvas_Bind_To_GLKView(std::string contextId){
    if( [GCanvasPlugin GetFetchPluginBlock] ){
        FetchPluginBlock block = [GCanvasPlugin GetFetchPluginBlock];
        GCanvasPlugin *plugin = block([NSString stringWithUTF8String:contextId.c_str()]);
        GLKView *glkview = [plugin getGLKView];
        [glkview bindDrawable];
    }
}


@end
