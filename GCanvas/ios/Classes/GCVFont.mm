/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#import "GCVFont.h"

#include <malloc/malloc.h>
#include <unordered_map>

#include "GFontStyle.h"
#include "GCanvas2dContext.h"

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
//    float txLineX, txLineY, txLineH;
    BOOL useSingleGlyphTextures;
    
    // Font preferences
    float pointSize, ascent, descent, contentScale, glyphPadding, xHeight;
    float lineWidth;
    
    CGGlyph *glyphsBuffer;
    CGPoint *positionsBuffer;
    std::unordered_map<int, GFontGlyphInfo> glyphInfoMap;//作为成员变量，是防止数据提前析构
    
    //font cache
    NSMutableDictionary *fontCache;
    
    NSString *curFontName;

}

@property (nonatomic, assign) BOOL isStroke;
@property (nonatomic, assign) CTFontRef ctMainFont;


@end

@implementation GCVFont
{
    NSString            *_key;
}

@synthesize context;
@synthesize glyphCache;
@synthesize treemap;

//+ (instancetype)sharedInstance{
//    static id sharedInstance = nil;
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        sharedInstance = [[self alloc] init];
//    });
//    return sharedInstance;
//}

//+ (void)setGCVFont:(GCVFont*)font forKey:(NSString*)key{
//    if( !staticFontInstaceDict ){
//        staticFontInstaceDict = NSMutableDictionary.dictionary;
//    }
//    staticFontInstaceDict[key] = font;
//}
//
//+ (GCVFont*)gcvFontObjectForKey:(NSString*)key{
//    return staticFontInstaceDict[key];
//}

static NSMutableDictionary *staticFontInstaceDict;
+ (instancetype)createGCFontWithKey:(NSString*)key
{
    if( !staticFontInstaceDict ){
        staticFontInstaceDict = NSMutableDictionary.dictionary;
    }
    GCVFont *gcvFont = [[GCVFont alloc] initWithKey:key];
    staticFontInstaceDict[key] = gcvFont;
    return gcvFont;
}

+ (GCVFont*)getGCVFontWithKey:(NSString*)key{
    return staticFontInstaceDict[key];
}

+ (void)removeGCVFontWithKey:(NSString*)key{
    return [staticFontInstaceDict removeObjectForKey:key];
}


- (instancetype)initWithKey:(NSString*)key{
    if( self = [super init] ){
        _key = key;
        fontCache = NSMutableDictionary.dictionary;
    }
    return self;
}

- (void)dealloc{
    if( self.ctMainFont ){
        CFRelease(self.ctMainFont);
    }
    fontCache = nil;
}

- (void)cleanFont{
    @synchronized(self){
        [GCVFont removeGCVFontWithKey:_key];
        [fontCache removeAllObjects];
    }
}

- (CTFontRef)createFontFromStyle:(gcanvas::GFontStyle*)fontStyle
{
    NSMutableDictionary *attrs = [NSMutableDictionary dictionary];
    //family
    NSString *family = [NSString stringWithUTF8String:fontStyle->GetFamily().c_str()];
    attrs[(NSString*)kCTFontFamilyNameAttribute] = family;
    
    //size
    attrs[(NSString*)kCTFontSizeAttribute] = @(fontStyle->GetSize());

    CTFontSymbolicTraits symbolicTraits = 0;
    if( fontStyle->GetWeight()==gcanvas::GFontStyle::Weight::BOLD )
    {
        symbolicTraits |= kCTFontBoldTrait;
    }
    
    BOOL isItalic = NO;
    if ( fontStyle->GetStyle()==gcanvas::GFontStyle::Style::ITALIC)
    {
        isItalic = YES;
        symbolicTraits |= kCTFontItalicTrait;
    }
    
    //kCTFontMonoSpaceTrait kCTFontCondensedTrait kCTFontExpandedTrait
    
    if (symbolicTraits)
    {
        NSDictionary *traits = @{(NSString*)kCTFontSymbolicTrait:@(symbolicTraits)};
        attrs[(NSString*)kCTFontTraitsAttribute] = traits;
    }
    
    CTFontDescriptorRef fontDesc = CTFontDescriptorCreateWithAttributes((CFDictionaryRef)attrs);
    
    CGAffineTransform matrix;
    if (isItalic)
    {
        matrix = CGAffineTransformMake(1, 0, tan(15*M_PI/180), 1, 0, 0);
    }
    
    CTFontRef curFont = CTFontCreateWithFontDescriptor(fontDesc, 0, isItalic ? &matrix : NULL );
    return curFont;
}

- (void)resetWithFontStyle:(gcanvas::GFontStyle *)fontStyle isStroke:(BOOL)isStroke context:(void*)context{
    
    self.isStroke = isStroke;
    GCanvasContext *canvasContext = (GCanvasContext*)context;
    contentScale = 1;
    lineWidth = canvasContext->LineWidth() * canvasContext->mDevicePixelRatio;
    useSingleGlyphTextures = true;
    
    glyphPadding = GCV_FONT_GLYPH_PADDING + (isStroke ? lineWidth : 0);
    
    const std::string& name = fontStyle->GetName();
    curFontName = [NSString stringWithUTF8String:name.c_str()];
    CTFontRef fontRef = (__bridge CTFontRef)([fontCache objectForKey:curFontName]);
    if( fontRef ){
        self.ctMainFont = fontRef;
        pointSize = fontStyle->GetSize();
        ascent = CTFontGetAscent(self.ctMainFont);
        descent = CTFontGetDescent(self.ctMainFont);
        xHeight = CTFontGetXHeight(self.ctMainFont);
        return;
    }
    
    CTFontRef curFont = [self createFontFromStyle:fontStyle];
    if (curFont){
        
        [fontCache setObject:(__bridge id)curFont forKey:curFontName];
        
        self.ctMainFont = curFont;
        pointSize = fontStyle->GetSize();
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


    GTextMetrics metrics = {
        .width = 0,
        .ascent = 0,
        .descent = 0,
    };
//    // Create a layout buffer large enough to hold all glyphs for this line
//    NSInteger lineGlyphCount = CTLineGetGlyphCount(line);
//    size_t layoutBufferSize = sizeof(GFontGlyphLayout) * lineGlyphCount;
    NSMutableData *layoutData = [NSMutableData dataWithCapacity:string.length];

//    // Create the layout object and add it to the cache
    GFontLayout *fontLayout = [[GFontLayout alloc] init];
    fontLayout.glyphLayout = layoutData;
    fontLayout.glyphCount = 0;
  
    int offsetX=0;

    
    NSInteger i,n=[string length];
    unichar c;
    for(i=0;i<n;i++){
        c=[string characterAtIndex:i];
        [self getGlyphForChar:c withFontStyle:fontStyle withFontLayout:fontLayout withOffsetX:&offsetX];
        ++fontLayout.glyphCount;
    }
    
   
    metrics.width=offsetX+2*glyphPadding;
    fontLayout.metrics = metrics;

    
    return fontLayout;
}

- (void )drawString:(NSString *)string withFontStyle:(NSString*)fontStyle withLayout:(GFontLayout*)fontLayout withPosition:(CGPoint)destPoint
{
    int offsetX=0;
    
    GColorRGBA color = self.isStroke ? BlendStrokeColor(context) : BlendFillColor(context);

    NSInteger i,n=[string length];
    unichar c;
    for(i=0;i<n;i++){
        c=[string characterAtIndex:i];
        
        const GGlyph *pGlyph = glyphCache->GetGlyph([fontStyle UTF8String], c, pointSize, self.isStroke);
        if( pGlyph ){
            GFontGlyphLayout gl;
            
            gl.info=*pGlyph;
            gl.xpos = offsetX;
            offsetX+=gl.info.advanceX;
            
            context->SetTexture(gl.info.texture);
            
            GGlyph &glyphInfo = gl.info;
            float gx = destPoint.x + gl.xpos + glyphInfo.offsetX;
            float gy = destPoint.y - (glyphInfo.height + glyphInfo.offsetY);
            context->PushRectangle(gx, gy, glyphInfo.width, glyphInfo.height, glyphInfo.s0,
                                   glyphInfo.t1, glyphInfo.s1-glyphInfo.s0, glyphInfo.t0-glyphInfo.t1,
                                   color);
        }
    }
}

- (void) getGlyphForChar:(wchar_t)c withFontStyle:(NSString*)fontStyle withFontLayout:(GFontLayout *) fontLayout withOffsetX:(int*) offsetX
{
    GTextMetrics metrics=fontLayout.metrics ;
    NSMutableData *layoutData = fontLayout.glyphLayout;

    const GGlyph *pGlyph = glyphCache->GetGlyph([fontStyle UTF8String], c, pointSize, self.isStroke);
    if (pGlyph)
    {
        GFontGlyphLayout gl;
        
        gl.info=*pGlyph;
        gl.xpos = *offsetX;
        *offsetX+=gl.info.advanceX;
        metrics.ascent = MAX(metrics.ascent, gl.info.height + gl.info.offsetY - glyphPadding);
        metrics.descent = MAX(metrics.descent, -gl.info.offsetY - glyphPadding);
        
        NSData * data = [NSData dataWithBytes:&gl length:sizeof(gl)];
        [layoutData appendData:data];
        return ;
        
    }

    
    GCVFont *curFont = self;
    
    NSString* nsString =[[NSString alloc] initWithBytes:&c length:sizeof(c) encoding:NSUTF32LittleEndianStringEncoding];
    
    if (!nsString) {
        return;
    }
    // Create attributed line
    NSAttributedString *attributes = [[NSAttributedString alloc]
                                      initWithString:nsString
                                      attributes:@{ (id)kCTFontAttributeName: (__bridge id)curFont.ctMainFont }];
    
    CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)attributes);
    

    // Create a layout buffer large enough to hold all glyphs for this line
//    NSInteger lineGlyphCount = CTLineGetGlyphCount(line);
    
    // Go through all runs for this line
    CFArrayRef runs = CTLineGetGlyphRuns(line);
    NSInteger runCount = CFArrayGetCount(runs);
    
    
    int layoutIndex = 0;
    for( int i = 0; i < runCount; i++ ) {
        CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, i);
//        CFRange str = CTRunGetStringRange(run);
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
            GFontGlyphLayout gl;

            CGGlyph glyph = glyphs[g];
            gl.info.charcode=c;
            gl.xpos = positions[g].x + *offsetX;

            [curFont createGlyph:glyph withFont:runFont withFontStyle:fontStyle glyphInfo:&gl.info];
            *offsetX+=gl.info.advanceX;
            metrics.ascent = MAX(metrics.ascent, gl.info.height + gl.info.offsetY - glyphPadding);
            metrics.descent = MAX(metrics.descent, -gl.info.offsetY - glyphPadding);
            layoutIndex++;
            
            NSData * data = [NSData dataWithBytes:&gl length:sizeof(gl)];
            [layoutData appendData:data];
        }
    }
    // Create the layout object and add it to the cache
    fontLayout.metrics = metrics;
    
    CFRelease(line);
    
}

- (void)createGlyph:(CGGlyph)glyph withFont:(CTFontRef)font withFontStyle:(NSString*)fontStyle glyphInfo:(GGlyph *)glyphInfo
{
    CGRect bbRect;
    CTFontGetBoundingRectsForGlyphs(font, kCTFontOrientationDefault, &glyph, &bbRect, 1);
    CGSize advance;
    CTFontGetAdvancesForGlyphs(font, kCTFontOrientationDefault, &glyph, &advance, 1);
    glyphInfo->advanceX=advance.width;
    glyphInfo->advanceY=advance.height;
    
    // Add some padding around the glyphs
    glyphInfo->offsetY = floorf(bbRect.origin.y) - glyphPadding;
    glyphInfo->offsetX = floorf(bbRect.origin.x) - glyphPadding;
    glyphInfo->width = bbRect.size.width + glyphPadding * 2;
    glyphInfo->height = bbRect.size.height + glyphPadding * 2;
    
    // Size needed for this glyph in pixels; must be a multiple of 8 for CG
    int pxWidth = floorf( (glyphInfo->width * contentScale)/8 + 1) * 8;
    int pxHeight = floorf( (glyphInfo->height * contentScale)/8 + 1) * 8;
    
 
    
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
    CGPoint p = CGPointMake(-glyphInfo->offsetX, -glyphInfo->offsetY);
    CTFontDrawGlyphs(font, &glyph, &p, 1, context);
    
    glyphInfo->width = pxWidth;
    glyphInfo->height = pxHeight;

        {
            glyphInfo->texture = nullptr;
            glyphInfo->bitmapBuffer = new unsigned char[pxWidth* pxHeight];
            memcpy(glyphInfo->bitmapBuffer, pixels.bytes, pxWidth* pxHeight);
            glyphCache->Insert([fontStyle UTF8String], glyphInfo->charcode, pointSize, self.isStroke, *glyphInfo);
        }
    CGContextRelease(context);
    
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
