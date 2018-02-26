/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#import "GCVCommon.h"
#import <OpenGLES/ES2/glext.h>  //define of GL_BGRA

@implementation GCVImageCache

@end


@interface GCVCommon()

@property(nonatomic, strong) NSMutableDictionary* hadLoadImageDict;


@end

@implementation GCVCommon

+ (instancetype)sharedInstance{
    GCVSharedInstanceIMP;
}

- (instancetype)init{
    self = [super init];
    if (self) {
        self.hadLoadImageDict = [[NSMutableDictionary alloc] init];
    }
    return self;
}

- (void)addPreLoadImage:(NSString *)imageStr completion:(void (^)(GCVImageCache*, BOOL))completion{
    GCVImageCache *imageCache = nil;
    @synchronized(self){
        imageCache = self.hadLoadImageDict[imageStr];
        
        if ( imageCache ){
            completion(imageCache, YES);
            return;
        }
        
        NSString *url = [imageStr hasPrefix:@"//"] ? [NSString stringWithFormat:@"https:%@", imageStr] : imageStr;
        NSURL *imageUrl = [NSURL URLWithString:url];
        
        if (!self.imageLoader) {
            completion(nil, NO);
            return;
        }
        
        GCVWeakSelf;
        [self.imageLoader loadImage:imageUrl completed:^(UIImage *image, NSError *error, BOOL finished, NSURL *imageURL) {
            GCVStrongSelfSafe;
            if (!error){
                GCVImageCache *imageCache = GCVImageCache.new;
                imageCache.image = image;
                
                CGImageRef cgimageRef = imageCache.image.CGImage;
                imageCache.width = CGImageGetWidth(cgimageRef);
                imageCache.height = CGImageGetHeight(cgimageRef);
                
                strongSelf.hadLoadImageDict[imageStr] = imageCache;
                completion(imageCache, NO);
            } else {
                completion(nil, NO);
            }
       }];
    }
}

- (GCVImageCache *)fetchLoadImage:(NSString *)imageStrkey{
    @synchronized(self){
        return self.hadLoadImageDict[imageStrkey];
    }
}

- (void)removeLoadImage:(NSString*)imageStrKey{
    @synchronized(self){
        [self.hadLoadImageDict removeObjectForKey:imageStrKey];
    }
}

- (void)clearLoadImageDict{
    @synchronized(self){
        [self.hadLoadImageDict removeAllObjects];
    }
}


+ (GLuint)bindTexture:(UIImage *)image{
    GCVLOG_METHOD(@"bindTexture image=%@", image);
    CGImageRef cgImageRef = [image CGImage];
    CGFloat width = (CGFloat)CGImageGetWidth(cgImageRef);
    CGFloat height = (CGFloat)CGImageGetHeight(cgImageRef);
    
    CGRect rect = CGRectMake(0, 0, width, height);
    CGBitmapInfo info = kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst;
    void* imageData = malloc(width * height * 4);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    CGContextRef context = CGBitmapContextCreate(imageData,width,height,8,4*width,colorSpace,info);
    CGContextClearRect(context, rect);
    CGContextDrawImage(context, rect, cgImageRef);
    
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);

    glEnable(GL_TEXTURE_2D);
    GLuint glID;
    glGenTextures(1, &glID);
    glBindTexture(GL_TEXTURE_2D, glID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    free(imageData);
    return glID;
}

@end
