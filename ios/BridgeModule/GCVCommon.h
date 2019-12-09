/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCVCommon_h
#define GCVCommon_h

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>
#import <CoreText/CoreText.h>
#import "GCVLog.h"

#define GCVWeakSelf    __weak __typeof(self) weakSelf = self;
#define GCVStrongSelf  __strong __typeof(weakSelf) strongSelf = weakSelf;
#define GCVStrongSelfSafe  GCVStrongSelf;if (!strongSelf) return;

#define GCVSharedInstanceIMP \
static id sharedInstance = nil; \
static dispatch_once_t onceToken; \
dispatch_once(&onceToken, ^{ \
sharedInstance = [[self alloc] init]; \
}); \
return sharedInstance;



/**
 * Called when image load finished.
 * @param image     The current image
 * @param error     error
 * @param finished  image load reuslt
 * @param imageURL  URL
 */
typedef void(^GCVLoadImageCompletion)(UIImage *image, NSError *error, BOOL finished, NSURL *imageURL);

/**
 * GCVImageLoaderProtocol, definiton load image Protocol.
 * plugin implement use SDWebImage
 */
@protocol GCVImageLoaderProtocol

@optional
/**
 *  Load image from url, call completion.
 *  @param  url         image url
 *  @param  completion  GCVLoadImageCompletion
 */
- (void)loadImage:(NSURL*)url completed:(GCVLoadImageCompletion)completion;
@end

/**
 * GCVImageCache, image relevant class with property, UIImage、textureId and id form JS image
 */
@interface GCVImageCache : NSObject


@property(nonatomic, assign) CGFloat width;

@property(nonatomic, assign) CGFloat height;

@property(nonatomic, strong) UIImage* image;

/**
 * UIImage bind to textureId, see GCVCommon:bindTexture.
 */
//@property(nonatomic, assign) GLuint textureId;

/**
 * An auto increase id, match with imageStr, from JavaScript
 */
@property (nonatomic, assign) NSUInteger jsTextreId;


@end


@interface GCVCommon : NSObject

/**
 * GCVImageLoaderProtocol delegate
 */
@property(nonatomic, weak) id<GCVImageLoaderProtocol> imageLoader;

+ (instancetype)sharedInstance;

/**
 * Bind a UIImage to a textureId.
 * @param image         current image
 *
 * return textureId
 */
+ (GLuint)bindTexture:(UIImage *)image;

/**
 * Create image with text 2d textureId.
 * @param target        [in] textureId
 * @param image         [out] UIImage
 */
//+ (void)textImage2D:(GLenum)target withImage:(UIImage *)image;

/**
 * Preload image with imageStr and idFromJS, when image load finished call completion with GCVImageCache.
 * @param imageStr      current image source
 * @param completion    callback preload
 */
- (void)addPreLoadImage:(NSString *)imageStr completion:(void (^)(GCVImageCache*, BOOL))completion;

/**
 * Fetch cached GCVImageCache with imageStr.
 * @param imageStrkey      current image source as key, format:instance_url
 *
 * return GCVImageCache object if exist, or nil
 */
- (GCVImageCache *)fetchLoadImage:(NSString *)imageStrkey;

/**
 * remove cached GCVImageCache with imageStr.
 * @param imageStrKey      current image source as key, format:instance_url
 *
 */
- (void)removeLoadImage:(NSString*)imageStrKey;

/**
 * Clear all preload and hadload image cache.
 */
- (void)clearLoadImageDict;

@end


#endif /* GCVCommon_h */


