/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#import "GCVFont.h"
#import "GCVCommon.h"

#include <malloc/malloc.h>
#include <unordered_map>


#define GCV_FONT_TEXTURE_SIZE 1024
#define GCV_FONT_GLYPH_PADDING 2

#pragma mark - GFontLayout
@implementation GFontLayout

@end

#pragma mark - GCVFontGlayphTexture
@interface GCVFontGlayphTexture : NSObject
@property(nonatomic, assign) short width;
@property(nonatomic, assign) short height;
@property(nonatomic, assign) GLuint textureId;
@end

@implementation GCVFontGlayphTexture

@end

#pragma mark - GCVFont
@interface GCVFont() {
    float txLineX, txLineY, txLineH;
    BOOL useSingleGlyphTextures;
    
    // Font preferences
    float pointSize, ascent, descent, contentScale, glyphPadding, xHeight;
    float lineWidth;
    
    CGGlyph *glyphsBuffer;
    CGPoint *positionsBuffer;
    std::unordered_map<int, GFontGlyphInfo> glyphInfoMap;//作为成员变量，是防止数据提前析构
    
    //font texture cache
    NSMutableDictionary *fontTextureCache;
    
    //font cache
    NSMutableDictionary *fontCache;
}

@property (nonatomic, assign) BOOL isStroke;
@property (nonatomic, assign) CTFontRef ctMainFont;

@end

@implementation GCVFont

+ (instancetype)sharedInstance{
    GCVSharedInstanceIMP;
}

- (instancetype)init{
    if( self = [super init] ){
        fontTextureCache = NSMutableDictionary.dictionary;
        fontCache = NSMutableDictionary.dictionary;
    }
    return self;
}

- (void)dealloc{
    CFRelease(self.ctMainFont);
    fontTextureCache = nil;
    fontCache = nil;
}

- (void)cleanFont{
    @synchronized(self){
        [fontTextureCache enumerateKeysAndObjectsUsingBlock:^(NSString *fontKey, NSMutableDictionary *textureMap, BOOL * _Nonnull stop) {
            [textureMap enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id glayphTextureId, BOOL * _Nonnull stop) {
                GLuint textureId = (GLuint)[glayphTextureId integerValue];
                glDeleteTextures(1, (const GLuint*)&textureId);
            }];
            [textureMap removeAllObjects];
        }];
        [fontTextureCache removeAllObjects];        
        [fontCache removeAllObjects];
    }
}

- (void)resetWithFontStyle:(const char *)fontStyle isStroke:(BOOL)isStroke deviceRatio:(CGFloat)deviceRatio{
    self.isStroke = isStroke;
    glyphPadding = GCV_FONT_GLYPH_PADDING + (isStroke ? lineWidth : 0);

    contentScale = deviceRatio;
    lineWidth=1;
    useSingleGlyphTextures = true;
    
    float size = 0;
    char name[64]="";
    char ptx;
    const char *start = fontStyle;
    while(*start != '\0' && !isdigit(*start)){ start++; } // skip to the first digit
    sscanf( start, "%fp%1[tx]%*[\"' ]%63[^\"']", &size, &ptx, name); // matches: 10.5p[tx] 'some font'
    
    if( ptx == 't' ) { // pt or px?
        size = ceilf(size*4.0/3.0);
    }
    
    NSString *fontStyleKey = [NSString stringWithUTF8String:fontStyle];
    CTFontRef fontRef = (__bridge CTFontRef)([fontCache objectForKey:fontStyleKey]);
    if( fontRef ){
        self.ctMainFont = fontRef;
        pointSize=size;
        ascent = CTFontGetAscent(self.ctMainFont);
        descent = CTFontGetDescent(self.ctMainFont);
        xHeight = CTFontGetXHeight(self.ctMainFont);
        return;
    }
    
    
    NSString *fontName = [NSString stringWithUTF8String:name];
    CTFontRef curFont = CTFontCreateWithName((CFStringRef)fontName, size, NULL);
    
    GCVLOG_D(@"fontStyle=%s, size=%.2f, ptx=%c, name=%s, curFont=%@", fontStyle, size, ptx, name, curFont);
    if (curFont){
        uint32_t boldFlag = strstr(fontStyle, "bold")?kCTFontBoldTrait:0;
        uint32_t italicFlag = (strstr(fontStyle, "italic")||strstr(fontStyle, "oblique"))?kCTFontTraitItalic:0;
        
        if (boldFlag||italicFlag){
            CTFontRef newFont = CTFontCreateCopyWithSymbolicTraits(curFont, 0.0, NULL, boldFlag|italicFlag, boldFlag|italicFlag);
            CFStringRef newName = CTFontCopyPostScriptName(newFont);
            GCVLOG_D(@"newName=%@", newName);
            CFRelease(curFont);
            curFont = CTFontCreateWithName(newName, size, NULL);
            CFRelease(newFont);
            CFRelease(newName);
        }
        
        [fontCache setObject:(__bridge id)curFont forKey:fontStyleKey];
        
        self.ctMainFont = curFont;
        pointSize=size;
        ascent = CTFontGetAscent(self.ctMainFont);
        descent = CTFontGetDescent(self.ctMainFont);
        xHeight = CTFontGetXHeight(self.ctMainFont);
        
        // If we can't fit at least two rows of glyphs into GCV_FONT_TEXTURE_SIZE, create
        // a new texture for each glyph. Otherwise we're wasting a lot of space.
        // This of course comes at the expense of a bit of performance, because we have
        // to bind a few more textures when drawing.
        if( (ascent + descent) * contentScale > GCV_FONT_TEXTURE_SIZE/2 ) {
            useSingleGlyphTextures = true;
        }
    }
}

- (GFontLayout *)getLayoutForString:(NSString *)string withFontStyle:(NSString*)fontStyle
{
    GCVFont *curFont = self;

    // Create attributed line
    NSAttributedString *attributes = [[NSAttributedString alloc]
                                      initWithString:string
                                      attributes:@{ (id)kCTFontAttributeName: (__bridge id)curFont.ctMainFont }];
    
    CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)attributes);
    
    // Get line metrics; sadly, ascent and descent are broken: 'ascent' equals
    // the total height (i.e. what should be ascent + descent) and 'descent'
    // is always the maximum descent of the font - no matter if you have
    // descending characters or not.
    // So, we have to collect those infos ourselfs from the glyphs.
    GTextMetrics metrics = {
        .width = static_cast<float>(CTLineGetTypographicBounds(line, NULL, NULL, NULL)),
        .ascent = 0,
        .descent = 0,
    };
    // Create a layout buffer large enough to hold all glyphs for this line
    NSInteger lineGlyphCount = CTLineGetGlyphCount(line);
    size_t layoutBufferSize = sizeof(GFontGlyphLayout) * lineGlyphCount;
    NSMutableData *layoutData = [NSMutableData dataWithLength:layoutBufferSize];
    GFontGlyphLayout *layoutBuffer = (GFontGlyphLayout *)layoutData.mutableBytes;
    
    // Go through all runs for this line
    CFArrayRef runs = CTLineGetGlyphRuns(line);
    NSInteger runCount = CFArrayGetCount(runs);
    
    
    int layoutIndex = 0;
    for( int i = 0; i < runCount; i++ ) {
        CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, i);
        NSInteger runGlyphCount = CTRunGetGlyphCount(run);
        CTFontRef runFont = (CTFontRef)CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
        // Fetch glyphs buffer
        const CGGlyph *glyphs = CTRunGetGlyphsPtr(run);
        if( !glyphs ) {
            size_t glyphsBufferSize = sizeof(CGGlyph) * runGlyphCount;
            if( malloc_size(glyphsBuffer) < glyphsBufferSize ) {
                glyphsBuffer = (CGGlyph *)realloc(glyphsBuffer, glyphsBufferSize);
            }
            CTRunGetGlyphs(run, CFRangeMake(0, 0), glyphsBuffer);
            glyphs = glyphsBuffer;
        }
        // Fetch Positions buffer
        CGPoint *positions = (CGPoint*)CTRunGetPositionsPtr(run);
        if( !positions ) {
            size_t positionsBufferSize = sizeof(CGPoint) * runGlyphCount;
            if( malloc_size(positionsBuffer) < positionsBufferSize ) {
                positionsBuffer = (CGPoint *)realloc(positionsBuffer, positionsBufferSize);
            }
            CTRunGetPositions(run, CFRangeMake(0, 0), positionsBuffer);
            positions = positionsBuffer;
        }
        
        // Go through all glyphs for this run, create the textures and collect the glyph
        // info and positions as well as the max ascent and descent
        for( int g = 0; g < runGlyphCount; g++ ) {
            GFontGlyphLayout *gl = &layoutBuffer[layoutIndex];
            gl->glyph = glyphs[g];
            gl->xpos = positions[g].x;
            gl->info = &glyphInfoMap[gl->glyph];
            gl->textureIndex = [curFont createGlyph:gl->glyph withFont:runFont withFontStyle:fontStyle glyphInfo:gl->info];
            metrics.ascent = MAX(metrics.ascent, gl->info->h + gl->info->y - glyphPadding);
            metrics.descent = MAX(metrics.descent, -gl->info->y - glyphPadding);
            layoutIndex++;
        }
    }
    // Create the layout object and add it to the cache
    GFontLayout *fontLayout = [[GFontLayout alloc] init];
    fontLayout.glyphLayout = layoutData;
    fontLayout.glyphCount = lineGlyphCount;
    fontLayout.metrics = metrics;
    
    CFRelease(line);
    
    return fontLayout;
}


- (GLuint)createGlyph:(CGGlyph)glyph withFont:(CTFontRef)font withFontStyle:(NSString*)fontStyle glyphInfo:(GFontGlyphInfo *)glyphInfo
{
    if( !fontTextureCache[fontStyle] ){
        fontTextureCache[fontStyle] = NSMutableDictionary.dictionary;
    }

    CGRect bbRect;
    CTFontGetBoundingRectsForGlyphs(font, kCTFontOrientationDefault, &glyph, &bbRect, 1);
    
    // Add some padding around the glyphs
    glyphInfo->y = floorf(bbRect.origin.y) - glyphPadding;
    glyphInfo->x = floorf(bbRect.origin.x) - glyphPadding;
    glyphInfo->w = bbRect.size.width + glyphPadding * 2;
    glyphInfo->h = bbRect.size.height + glyphPadding * 2;
    
    // Size needed for this glyph in pixels; must be a multiple of 8 for CG
    int pxWidth = floorf((glyphInfo->w * contentScale) / 8 + 1) * 8;
    int pxHeight = floorf((glyphInfo->h * contentScale) / 8 + 1) * 8;
    
    // Do we need to create a new texture to hold this glyph?
    NSMutableDictionary *textureCache = fontTextureCache[fontStyle];
    BOOL createNewTexture = (textureCache.count == 0 || useSingleGlyphTextures);
    
    if( txLineX + pxWidth > GCV_FONT_TEXTURE_SIZE ) {
        // New line
        txLineX = 0.0f;
        txLineY += txLineH;
        txLineH = 0.0f;
        
        // Line exceeds texture height? -> new texture
        if( txLineY + pxHeight > GCV_FONT_TEXTURE_SIZE) {
            createNewTexture = YES;
        }
    }
    
    txLineX = txLineY = txLineH = 0;
    int textureWidth = GCV_FONT_TEXTURE_SIZE, textureHeight = GCV_FONT_TEXTURE_SIZE;
    
    // In single glyph mode, create a new texture for each glyph in the glyph's size
    if( useSingleGlyphTextures ) {
        textureWidth = pxWidth;
        textureHeight = pxHeight;
    }
    
    GLuint textureId = (GLuint)([textureCache[@(glyph)] integerValue]);
    if( textureId == 0 ) {
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        textureCache[@(glyph)] = @(textureId);
    } else {
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
    
    glyphInfo->textureIndex = textureCache.count; // 0 is reserved, index starts at 1
    glyphInfo->tx = ((txLineX+1) / textureWidth);
    glyphInfo->ty = ((txLineY+1) / textureHeight);
    glyphInfo->tw = (glyphInfo->w / textureWidth) * contentScale,
    glyphInfo->th = (glyphInfo->h / textureHeight) * contentScale;
    
    NSMutableData *pixels = [NSMutableData dataWithLength:pxWidth * pxHeight];
    
    CGContextRef context = CGBitmapContextCreate(pixels.mutableBytes, pxWidth, pxHeight, 8, pxWidth, NULL, kCGImageAlphaOnly);
    
    CGContextSetFontSize(context, pointSize);
    CGContextTranslateCTM(context, 0.0, pxHeight);
    CGContextScaleCTM(context, contentScale, -1.0*contentScale);
    
    // Fill or stroke?
    if( !self.isStroke ) {
        CGContextSetTextDrawingMode(context, kCGTextFill);
        CGContextSetGrayFillColor(context, 1.0, 1.0);
    } else {
        CGContextSetTextDrawingMode(context, kCGTextStroke);
        CGContextSetGrayStrokeColor(context, 1.0, 1.0);
        CGContextSetLineWidth(context, lineWidth);
    }
    
    // Render glyph and update the texture
    CGPoint p = CGPointMake(-glyphInfo->x, -glyphInfo->y);
    CTFontDrawGlyphs(font, &glyph, &p, 1, context);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, pxWidth, pxHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels.bytes);

    // Update texture coordinates
    txLineX += pxWidth;
    txLineH = MAX( txLineH, pxHeight );
    
    CGContextRelease(context);
    
    return textureId;
}

-(CGPoint)adjustTextPenPoint:(CGPoint)srcPoint
                   textAlign:(GTextAlign)textAlign
                    baseLine:(GTextBaseline)baseLine
                     metrics:(GTextMetrics)metrics
{
    switch (textAlign) {
        case GTextAlign::TEXT_ALIGN_START:
        case GTextAlign::TEXT_ALIGN_LEFT:
            break;
        case GTextAlign::TEXT_ALIGN_CENTER:
            srcPoint.x -= metrics.width/2.0f;
            break;
        case GTextAlign::TEXT_ALIGN_END:
        case GTextAlign::TEXT_ALIGN_RIGHT:
            srcPoint.x -= metrics.width;
            break;
    }
    
    switch( baseLine ) {
        case GTextBaseline::TEXT_BASELINE_ALPHABETIC:
        case GTextBaseline::TEXT_BASELINE_IDEOGRAPHIC:
            break;
        case GTextBaseline::TEXT_BASELINE_TOP:
        case GTextBaseline::TEXT_BASELINE_HANGING:
            srcPoint.y += ascent;
            break;
        case GTextBaseline::TEXT_BASELINE_MIDDLE:
            srcPoint.y += xHeight/2;
            break;
        case GTextBaseline::TEXT_BASELINE_BOTTOM:
            srcPoint.y -= descent;
            break;
    }
    
    return srcPoint;
}

@end
