/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCVFont_h
#define GCVFont_h

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>
#import <CoreText/CoreText.h>
#import "GCVLog.h"
#include "GTextDefine.h"

typedef struct {
    float x, y, w, h;
    unsigned short textureIndex;
    float tx, ty, tw, th;
} GFontGlyphInfo;

typedef struct {
    unsigned short textureIndex;
    CGGlyph glyph;
    float xpos;
    GFontGlyphInfo *info;
} GFontGlyphLayout;

typedef struct {
    float width;
    float ascent;
    float descent;
} GTextMetrics;


@interface GFontLayout : NSObject

@property (nonatomic, strong) NSData *glyphLayout;
@property (nonatomic, assign) GTextMetrics metrics;
@property (nonatomic, assign) NSInteger glyphCount;

@end


@interface GCVFont : NSObject 

+ (instancetype)sharedInstance;

- (void)cleanFont;

/**
 * Reset GCVFont style with fontStyle, isStroke.
 *
 * @param fontStyle     fontStyle name
 * @param isStroke      stroke flag
 */
- (void)resetWithFontStyle:(const char *)fontStyle isStroke:(BOOL)isStroke deviceRatio:(CGFloat)deviceRatio;

/**
 * Get getLayoutForString.
 *
 * @param string        input string
 *
 * return  return GFontLayout
 */
- (GFontLayout *)getLayoutForString:(NSString *)string withFontStyle:(NSString*)fontStyle;

/**
 * Create font texture with FontGlyph info
 *
 * @param glyph         CGGlyph
 * @param font          CTFontRef
 * @param fontStyle     NSString
 * @param glyphInfo     GFontGlyphInfo
 *
 * return  font textureId
 */
- (GLuint)createGlyph:(CGGlyph)glyph
             withFont:(CTFontRef)font
        withFontStyle:(NSString*)fontStyle
            glyphInfo:(GFontGlyphInfo *)glyphInfo;


/**
 * Adjust text source point
 *
 * @param srcPoint          source point to adjust
 * @param textAlign         see GTextAlign definition
 * @param baseLine          see GTextBaseline definition
 * @param metrics           see GTextMetrics definition
 *
 * return  adjuested text source CGPoint
 */
-(CGPoint)adjustTextPenPoint:(CGPoint)srcPoint
                   textAlign:(GTextAlign)textAlign
                    baseLine:(GTextBaseline)baseLine
                     metrics:(GTextMetrics)metrics;

@end

#endif /* GCVFont_h */
