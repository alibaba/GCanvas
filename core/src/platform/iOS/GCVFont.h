/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCVFONT_H
#define GCANVAS_GCVFONT_H

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>
#import <CoreText/CoreText.h>
#include "GTextDefine.h"
#include "GGlyphCache.h"
#include "GTreemap.h"

typedef struct {
    float x, y, w, h;
    CGSize advances;
    unsigned short textureIndex;
    float tx, ty, tw, th;
} GFontGlyphInfo;


typedef struct {
    float xpos;
    GGlyph info;
} GFontGlyphLayout;

typedef struct {
    float width;
    float ascent;
    float descent;
} GTextMetrics;

namespace gcanvas
{
    class GFontStyle;
}

@interface GFontLayout : NSObject

@property (nonatomic, strong) NSMutableData *glyphLayout;
@property (nonatomic, assign) GTextMetrics metrics;
@property (nonatomic, assign) NSInteger glyphCount;

@end

class GCanvasContext;

@interface GCVFont : NSObject

@property(nonatomic,assign) GGlyphCache *glyphCache;
@property(nonatomic,assign) GTreemap *treemap;

+ (instancetype)createGCFontWithKey:(NSString*)key;
+ (GCVFont*)getGCVFontWithKey:(NSString*)key;


- (void)cleanFont;

/**
 * Reset GCVFont style with fontStyle, isStroke.
 *
 * @param fontStyle     fontStyle name
 * @param isStroke      stroke flag
 */
- (void)resetWithFontStyle:(gcanvas::GFontStyle *)fontStyle isStroke:(BOOL)isStroke context:(GCanvasContext *)context;

/**
 * Get getLayoutForString.
 *
 * @param string        input string
 *
 * return  return GFontLayout
 */
- (GFontLayout *)getLayoutForString:(NSString *)string
                       withFontName:(NSString*)fontName;

- (void )drawString:(NSString*)string
       withFontName:(NSString*)fontName
         withLayout:(GFontLayout*)fontLayout
       withPosition:(CGPoint)destPoint
            context:(GCanvasContext *)context;

- (void)getGlyphForChar:(wchar_t)c
           withFontName:(NSString*)fontName
         withFontLayout:(GFontLayout *)fontLayout
            withOffsetX:(int*)x;

/**
 * Create font texture with FontGlyph info
 *
 * @param glyph         CGGlyph
 * @param font          CTFontRef
 * @param fontName     NSString
 * @param glyphInfo     GFontGlyphInfo
 *
 * return  font textureId
 */
- (void)createGlyph:(CGGlyph)glyph
           withFont:(CTFontRef)font
       withFontName:(NSString*)fontName
          glyphInfo:(GGlyph *)glyphInfo;


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
- (CGPoint)adjustTextPenPoint:(CGPoint)srcPoint
                   textAlign:(GTextAlign)textAlign
                    baseLine:(GTextBaseline)baseLine
                     metrics:(GTextMetrics)metrics;

- (NSString *)getFontNameWithCurrentScale:(gcanvas::GFontStyle *)fontStyle
                                  context:(GCanvasContext *)context;

@end

#endif /* GCANVAS_GCVFONT_H */
