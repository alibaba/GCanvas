/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

typedef NS_ENUM(NSUInteger, GCVContextType){
    GCVContextType2D    = 0,
    GCVContextTypeWebGL = 1
};

@protocol GCVImageLoaderProtocol;

@interface GCanvasPlugin : NSObject

/**
 *  @abstract       set LogLevel
 *
 *  @param logLevel see definiation gcanvas::LogLevel, 0-debug, 1-info, 2-warn, 3-error
 */
+ (void)setLogLevel:(NSUInteger)logLevel;

/**
 *  @abstract   init GCanvas with componentId
 *  @param      componentId   unique instance bind GCanvas
 */
- (instancetype)initWithComponentId:(NSString*)componentId;

/**
 *  @abstract   set Canvas frame
 *  @param      frame   frame of Canvas
 */
- (void)setFrame:(CGRect)frame;

/**
 *  @abstract   set Canvas clear color
 *  @param      color   clear color value
 */
- (void)setClearColor:(UIColor*)color;

/**
 *  @abstract   add commamds received from JS Bridge "render"
 *  @param  commands    commands from JS
 */
- (void)addCommands:(NSString *)commands;

/**
 *  @abstract   execute command received from JS Bridge "render"
 */
- (void)execCommands;

/**
 *  @abstract   remove commands after execute
 */
- (void)removeCommands;

/**
 *  @abstract   release manager when release webview
 */
- (void)releaseManager;

/**
 *  @abstract   fetch image texture id in this GCanvas context
 *
 *  @param  aid         id from js
 */
- (GLuint)getTextureId:(NSUInteger)aid;

/**
 *  @abstract   binding image with texture id, id from JS and image size.
 *
 *  @param  tid         texture id
 *  @param  aid         id from js
 *  @param  width       image width
 *  @param  height      image height
 *  @param  offscreen   tid is offline texture, while offscreen=YES
 */
- (void)addTextureId:(NSUInteger)tid withAppId:(NSUInteger)aid width:(NSUInteger)width height:(NSUInteger)height offscreen:(BOOL)offscreen;

/**
 *  @abstract   binding image with texture id, id from JS and image size.
 *
 *  @param  tid         texture id
 *  @param  aid         id from js
 *  @param  width       image width
 *  @param  height      image height
 */
- (void)addTextureId:(NSUInteger)tid withAppId:(NSUInteger)aid width:(NSUInteger)width height:(NSUInteger)height;

/**
 *  @abstract   native download image
 *
 *  @param      loader impletement GCVImageLoaderProtocol protocol
 */
- (void)setImageLoader:(id<GCVImageLoaderProtocol>)loader;

/**
 *  @abstract   set pixel rate after recived from JS
 *
 *  @param      ratio   value
 */
- (void)setDevicePixelRatio:(double)ratio;

/**
 *  @abstract   set GCanvas context type
 *
 *  @param contextType      see GCVContextType definiation
 */
- (void)setContextType:(GCVContextType)contextType;

/**
 *  @abstract   get GCanvas context type
 *
 *  @return     current context type, see GCVContextType definiation
 */
- (int)contextType;

/**
 *  @abstract   get GCanvas fps
 *
 *  @return     fps
 */
- (CGFloat)fps;

/**
 *  @abstract   get GCanvas textureId
 *
 *  @return     fbo textureId
 */
- (GLuint)textureId;

/**
 *  @abstract   explicit remove GCanvas
 */
- (void)removeGCanvas;

/**
 *  @abstract   get webgl Sync call result
 */
- (NSString*)getSyncResult;

@end
